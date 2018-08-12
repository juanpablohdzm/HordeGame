// Fill out your copyright notice in the Description page of Project Settings.

#include "Powerups/SHealthRegenerator.h"
#include "MyComponents/SHealthComponent.h"
#include <Components/StaticMeshComponent.h>




ASHealthRegenerator::ASHealthRegenerator()
{
	HealAmount = 20.0f;
}

void ASHealthRegenerator::OnActivated(AActor * OtherActor)
{
	Super::OnActivated(OtherActor);
	if (MeshComp)
	{
		MeshComp->SetVisibility(false);
	}
}

void ASHealthRegenerator::OnPowerupTicked()
{
	if (ActiveActor)
	{
		USHealthComponent* HealthComp=Cast<USHealthComponent>(ActiveActor->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp)
		{
			HealthComp->Heal(HealAmount);
		}
		
	}
}

void ASHealthRegenerator::OnExpired()
{
	Destroy();
}
