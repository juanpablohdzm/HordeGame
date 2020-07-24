// Fill out your copyright notice in the Description page of Project Settings.

#include "SExplosiveBarrel.h"
#include <Components/StaticMeshComponent.h>
#include "MyComponents/SHealthComponent.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/DamageType.h>
#include <PhysicsEngine/RadialForceComponent.h>
#include <Materials/Material.h>
#include <Net/UnrealNetwork.h>



// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	//Set to physics body to let radial component affect us (eg. when a nerby barrel explodes)
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = MeshComp;

	
	//Create a default component to do a radial impulse
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->Radius = 250.0f;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->bIgnoreOwningActor = true;

	ExplosionImpulse = 400;
	BaseDamage = 50.0f;

	SetReplicates(true);
	SetReplicateMovement(true);

}



void ASExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChange);
}

void ASExplosiveBarrel::OnHealthChange(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (Health <= 0.0f && !bExploded)
		{
			//Explode
			bExploded = true;
			//This function is called in case the server is also a host! Play fire effects
			OnRep_Exploded();

			//Boost the barrel upwards
			FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
			MeshComp->AddImpulse(BoostIntensity, NAME_None, true);



			//Blast away nearby physics actors
			RadialForceComp->FireImpulse();

			//Do damage
			UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), RadialForceComp->Radius, UDamageType::StaticClass(), TArray<AActor*>(), this, nullptr, true);
		}
	}
}

void ASExplosiveBarrel::OnRep_Exploded()
{
	//Play FX and change self material to black 
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}
	//Override material on mesh 
	if (ExplodedMaterial)
	{
		MeshComp->SetMaterial(0, ExplodedMaterial);
	}
}

void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASExplosiveBarrel, bExploded);
}


