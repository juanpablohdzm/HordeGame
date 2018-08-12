// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"
#include <Components/StaticMeshComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundWave.h>
#include <Net/UnrealNetwork.h>





// Sets default values
ASPowerupActor::ASPowerupActor()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PowerupInterval = 0.0f;
	TotalNrOfTicks = 0;
	bIsPowerActive = false;

	SetReplicates(true);

}


void ASPowerupActor::ActivatePowerup(AActor *OtherActor)
{
	//This is only called on the server due to pickupactor notify on actor begin overlap...
	bIsPowerActive = true;//...that is why we use a ReplicatedUsing variable instead of a ServerFunction, this will never be called by a Role <Role_Authority
	ActiveActor = OtherActor;
	OnRep_IsActive();
	
}

void ASPowerupActor::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();
	OnPowerupTicked_BP();
	if (TicksProcessed >= TotalNrOfTicks)
	{
		OnExpired();
		OnExpired_BP();

		//Delete timer
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void ASPowerupActor::OnActivated(AActor * OtherActor)
{
	if(SoundActivate)
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundActivate, GetActorLocation());
}

void ASPowerupActor::OnExpired()
{
	bIsPowerActive = false;
}

void ASPowerupActor::OnPowerupTicked()
{

}


void ASPowerupActor::OnRep_IsActive()
{
	if (ActiveActor && bIsPowerActive)
	{
		OnActivated(ActiveActor);
		OnActivated_BP(ActiveActor);
		if (PowerupInterval > 0.0f)
			GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInterval, true);
		else
			OnTickPowerup();
	}
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupActor, bIsPowerActive);
	DOREPLIFETIME(ASPowerupActor, ActiveActor);
}

