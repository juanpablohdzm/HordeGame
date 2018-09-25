// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"

class ASPowerupActor;

UCLASS()
class HORDEGAME_API ASPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*
	Components and variables
	*/
	UPROPERTY(VisibleAnywhere,Category="Components")
	class USphereComponent * SphereComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UDecalComponent * DecalComp;

	UPROPERTY(EditInstanceOnly,Category="PickupActor")
	TSubclassOf<ASPowerupActor> PowerupClass;
	
	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	float SpawnWaitTime;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	TSubclassOf<AActor> PlayerClass;
	
	ASPowerupActor* PowerupInstance;

	FTimerHandle TimerHandle_RespawnTimer;
	/*
	Functions
	*/
	UFUNCTION()
	void RespawnPowerup();



public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	
};
