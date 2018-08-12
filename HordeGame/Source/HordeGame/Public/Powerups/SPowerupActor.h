// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class HORDEGAME_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent * MeshComp;

protected:
	/*
	Variables
	*/
	UPROPERTY(EditDefaultsOnly,Category="Powerups")
	class USoundWave * SoundActivate;

	/*Time between powerup spawns*/
	UPROPERTY(EditDefaultsOnly,Category="Powerups")
	float PowerupInterval;

	/* Total time we apply the powerup effect*/
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 TotalNrOfTicks;

	/*Total number of ticks applied*/
	int32 TicksProcessed;

	FTimerHandle TimerHandle_PowerupTick;


	//Check
	UPROPERTY(Replicated)
	AActor * ActiveActor;

	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
	bool bIsPowerActive;


	
public:
	/*
	Functions
	*/

	void ActivatePowerup(AActor * OtherActor);

	float GetPowerupInterval() const { return PowerupInterval; }
	void SetPowerupInterval(float val) { PowerupInterval = val; }
protected:
	UFUNCTION(BlueprintImplementableEvent,Category="Powerups")
	void OnActivated_BP(AActor * OtherActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired_BP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked_BP();

	UFUNCTION()
	void OnTickPowerup();


	virtual void OnActivated(AActor * OtherActor);

	virtual void OnExpired();
	
	virtual void OnPowerupTicked();

	UFUNCTION()
	void OnRep_IsActive();


};
