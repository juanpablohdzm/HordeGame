// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Powerups/SPowerupActor.h"
#include "SHealthRegenerator.generated.h"

/**
 * 
 */
UCLASS()
class HORDEGAME_API ASHealthRegenerator : public ASPowerupActor
{
	GENERATED_BODY()
public:
	ASHealthRegenerator();
	
protected:
	/*
	Variables
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float HealAmount;
	/*
	Functions
	*/

	virtual void OnActivated(AActor * OtherActor) override;
	virtual void OnPowerupTicked() override;
	virtual void OnExpired() override;
	
	
};
