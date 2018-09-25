// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/STrackerBot.h"
#include <Components/StaticMeshComponent.h>
#include <Kismet/GameplayStatics.h>
#include <NavigationSystem.h>
#include <NavigationPath.h>
#include <GameFramework/Character.h>
#include <Materials/MaterialInstanceDynamic.h>
#include "MyComponents/SHealthComponent.h"
#include "SCharacter.h"
#include <Components/SphereComponent.h>
#include <Sound/SoundCue.h>
#include <WorldCollision.h>

// Sets default values
ASTrackerBot::ASTrackerBot()
{
	Damage = 60.0f;
	SelfDamage = 20.0f;
	DamageInterval = 0.5f;
	DamageRadius = 350.0f;
	MovementForce = 1000.0f;
	RequiredDistanceToTarget = 100.0f;
	bUseVelocityChange = true;
	bExploded = false;
	bStartedTimerDestruction = false;
	PowerLevel = 1;

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	RootComponent = MeshComp;
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("Health Comp"));

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Comp"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore); //Ignore everyone but...
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); //... my enemy, this just overlap don´t push. 
	SphereComp->SetSphereRadius(DamageRadius);
	SphereComp->SetupAttachment(MeshComp);



}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		//Find initial go to
		NextPathPoint = GetNextPathPoint();

		FTimerHandle TimerHandle_CheckPowerLevel;
		GetWorldTimerManager().SetTimer(TimerHandle_CheckPowerLevel, this, &ASTrackerBot::OnCheckNearbyBots, 1.0f, true);
	}
	
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority && !bExploded)
	{
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();
		}
		else
		{
			//keep moving towards next target

			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();

			ForceDirection *= MovementForce;

			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
		}
	}

}


void ASTrackerBot::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);
}


void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!bStartedTimerDestruction && !bExploded)
	{
		ASCharacter * PlayerPawn = Cast<ASCharacter>(OtherActor);
		if (PlayerPawn  && !USHealthComponent::IsFriendly(OtherActor,this))
		{
			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
			//We overlapped with a player

			if (Role == ROLE_Authority)
			{
				//Self destruction sequence
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, DamageInterval, true, 0.0f);
			}
				bStartedTimerDestruction = true;
			
		}
		
	}
}

FVector ASTrackerBot::GetNextPathPoint()
{
	AActor * NearestTarget=nullptr;
	float TargetDistance=FLT_MAX;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It;++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || USHealthComponent::IsFriendly(TestPawn,this)) { continue; }
		USHealthComponent* TestPawnHealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (TestPawnHealthComp && TestPawnHealthComp->GetCurrentHealth() > 0.0f)
		{
			float Distance = (TestPawn->GetActorLocation() - GetActorLocation()).Size();
			if (TargetDistance>Distance)
			{
				NearestTarget = TestPawn;
				TargetDistance = Distance;
			}
		}
	}
	if (NearestTarget)
	{
		UNavigationPath * NavigationPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), NearestTarget);

		GetWorldTimerManager().ClearTimer(TimerHandle_RefreshPath);
		GetWorldTimerManager().SetTimer(TimerHandle_RefreshPath, this, &ASTrackerBot::RefreshPath, 5.0f, false);


		if (NavigationPath && NavigationPath->PathPoints.Num() > 1)
		{
			//Return next point in the path
			return NavigationPath->PathPoints[1];
		}
	}
	return GetActorLocation();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//This only happens in the server because this OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage); only happens in the server 

	//Pulse the material on hit
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	//Explode on hitpoints ==0
	if (Health<=0.0f)
	{
		SelfDestruct();
	}
	
	

}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}
	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());

	MeshComp->SetVisibility(false,true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Role == ROLE_Authority)
	{
		TArray<AActor*>IgnoredActors;
		IgnoredActors.Add(this);

		UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage*PowerLevel, GetActorLocation(), DamageRadius, UDamageType::StaticClass(), IgnoredActors, this, GetInstigatorController(), true);

		SetLifeSpan(2.0f);
	}
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, SelfDamage, GetInstigatorController(), this, UDamageType::StaticClass());
}

void ASTrackerBot::OnCheckNearbyBots()
{
	//distance to check for nearby bots
	const float Radius = 600.0f;

	//Create temporary collision shape for overlaps
	FCollisionShape CollShape;
	CollShape.SetSphere(Radius);

	//Only find Pawns
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);

	int32 NrOfBots = 1;

	for (FOverlapResult Result: Overlaps)
	{
		ASTrackerBot*Bot = Cast<ASTrackerBot>(Result.GetActor());

		if (Bot && Bot!=this)
		{
			NrOfBots++;
		}
	}

	const int32 MaxPowerLevel = 4;

	PowerLevel = FMath::Clamp(NrOfBots, 1, MaxPowerLevel);

	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		float Alpha = PowerLevel / (float)MaxPowerLevel;
		MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	}



}

void ASTrackerBot::RefreshPath()
{
	NextPathPoint = GetNextPathPoint();
}

