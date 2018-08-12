// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Powerups/SPowerupActor.h"
#include "SSuperspeed.generated.h"

/**
 * 
 */

UCLASS()
class HORDEGAME_API ASSuperspeed : public ASPowerupActor
{
	GENERATED_BODY()

public:

	ASSuperspeed();

	/*
	 Variables
	 */
	
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float SpeedMultiplier;

	/*
	Functions
	*/
protected:

	virtual void OnActivated(AActor * OtherActor) override;

	virtual void OnExpired() override;

	virtual void OnPowerupTicked() override;	
	
};

