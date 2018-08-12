// Fill out your copyright notice in the Description page of Project Settings.


#include "Powerups/SSuperspeed.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Powerups/SPowerupActor.h"
#include <Components/StaticMeshComponent.h>




ASSuperspeed::ASSuperspeed()
{
	SpeedMultiplier = 2.0f;
}

void ASSuperspeed::OnActivated(AActor * OtherActor)
{
	Super::OnActivated(OtherActor);
	if (MeshComp)
	{
		MeshComp->SetVisibility(false);
	}
	if (ActiveActor)
	{
		UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(ActiveActor->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
		if (MovementComponent)
		{
			MovementComponent->MaxWalkSpeed *= SpeedMultiplier;
		}
	
	}

}

void ASSuperspeed::OnExpired()
{
	
	if (ActiveActor)
	{
		UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(ActiveActor->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
		MovementComponent->MaxWalkSpeed /= SpeedMultiplier;
	}
	Destroy();

}

void ASSuperspeed::OnPowerupTicked()
{

}



