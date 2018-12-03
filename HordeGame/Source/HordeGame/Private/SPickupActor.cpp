// Fill out your copyright notice in the Description page of Project Settings.

#include "SPickupActor.h"
#include <Components/SphereComponent.h>
#include <Components/DecalComponent.h>
#include <Engine/World.h>
#include <ConstructorHelpers.h>
#include "Powerups/SPowerupActor.h"
#include "SCharacter.h"


// Sets default values
ASPickupActor::ASPickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Comp"));
	RootComponent = SphereComp;
	SphereComp->SetSphereRadius(75.0f);

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal Comp"));
	DecalComp->SetupAttachment(RootComponent);
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64, 75, 75);


	static ConstructorHelpers::FClassFinder<AActor> PlayerPawnClassFinder(TEXT("/Game/Characters/BP/BP_PlayerPawn"));
	PlayerClass = PlayerPawnClassFinder.Class;

	SpawnWaitTime = 10.0f;

	SetReplicates(true);
	
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	if (Role == ROLE_Authority)
	{
		RespawnPowerup();
	}
}

void ASPickupActor::RespawnPowerup()
{
	if (!PowerupClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("No powerup class to spawn"));
		return;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerupInstance = GetWorld()->SpawnActor<ASPowerupActor>(PowerupClass,GetActorLocation() +FVector(0,0,50.0f),GetActorRotation(), SpawnParams);
}



void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (OtherActor->IsA(PlayerClass) && Role == ROLE_Authority && PowerupInstance)
	{
		
		PowerupInstance->ActivatePowerup(OtherActor);
		//Set timer to respawn
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPickupActor::RespawnPowerup, SpawnWaitTime);

		PowerupInstance = nullptr;
		
	}
}


