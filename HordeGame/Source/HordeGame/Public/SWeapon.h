// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"


class UParticleSystem;


//Contains information of a single hitscan weapon linetrace 
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()
public:

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UCLASS()
class HORDEGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	virtual void BeginPlay() override;



protected:
	//Components and data

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	class USkeletalMeshComponent * MeshComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem * SmokeTrail;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="FX")
	UParticleSystem * MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem * DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem * FleshImpactEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class UCameraShake> FireCamShake;


	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TraceTargetName;

	UPROPERTY(EditDefaultsOnly,Category="Damage")
	float BaseDamage;

	/*RPM -Bullets per minute fired by weapon*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	float LastFireTime;

	//Derived from Rate of fire
	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapon",meta=(ClampMax=100,ClampMin=0))
	int MaxAmmo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 0))
	int MaxRound;

	UPROPERTY(Replicated)
	int CurrentAmmo;

	UPROPERTY(Replicated)
	int CurrentRound;

	/*Bullet spread in degrees*/
	UPROPERTY(EditDefaultsOnly,Category="Weapon",meta=(ClampMin=0.0f))
	float BulletSpreadAngle;
	
	//Every time this variable change it will trigger the function 
	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	FTimerHandle TimerHandle_TimeBetweenShots;

protected:
	//Functions

	void PlayFireEffects(FVector TraceEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceType , FVector ImpactPoint);

	virtual void Fire();

	UFUNCTION(Server,Reliable,WithValidation)
	virtual void ServerFire();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerStopFire();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerReload();

	UFUNCTION()
	virtual void OnRep_HitScanTrace();

	

public:	

	UFUNCTION(BlueprintCallable, Category = "VR")
	virtual void StopFire();

	UFUNCTION(BlueprintCallable, Category = "VR")
	virtual void StartFire();

	UFUNCTION(BlueprintCallable, Category = "VR")
	virtual void Reload();

};
