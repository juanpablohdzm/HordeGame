// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

UCLASS()
class HORDEGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	/*
	Components and variables 
	*/
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class UStaticMeshComponent * MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class USHealthComponent * HealthComp;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class USphereComponent * SphereComp;

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float SelfDamage;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float DamageInterval;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float DamageRadius;

	UPROPERTY(EditDefaultsOnly,Category="TrackerBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float RequiredDistanceToTarget;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool bUseVelocityChange;

	//Dynamic material to pulse 
	class UMaterialInstanceDynamic* MatInst;

	UPROPERTY(EditDefaultsOnly,Category="TrackerBot")
	class UParticleSystem * ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	class USoundCue * SelfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	class USoundCue * ExplosionSound;

	bool bExploded;

	bool bStartedTimerDestruction;

	FTimerHandle TimerHandle_SelfDamage;

	FTimerHandle TimerHandle_RefreshPath;
	
	int32 PowerLevel;


	/*
	Functions
	*/
	virtual void PostInitializeComponents() override;

	FVector GetNextPathPoint();

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void SelfDestruct();

	void DamageSelf();


	void OnCheckNearbyBots();

	void RefreshPath();
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	
};
