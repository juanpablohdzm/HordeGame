// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

UCLASS()
class HORDEGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	virtual void PostInitializeComponents() override;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

protected:
	/*
	*Components
	*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	class USpringArmComponent * SpringArmComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	class UCameraComponent * CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USHealthComponent * HealthComp;

	UPROPERTY(EditDefaultsOnly,Category="Player")
	TSubclassOf<AActor> RifleClass;

	UPROPERTY(Replicated)
	class ASWeapon * Weapon;

	/*
	*Variables
	*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
	float ZoomInterpSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomFOV;

	/*Default FOV set during begin  play*/
	float DefaultFOV;

	/* Pawn life status*/
	UPROPERTY(Replicated,BlueprintReadOnly,Category="Player")
	bool bDied;


public:
	//This is done for the AI know when it has to reload it's weapon. 
	UPROPERTY(BlueprintReadOnly,Category="AI")
	int BulletCount = 0;

	

protected:
	/*
	*Input Functions
	*/

	void MoveForward(float value);

	void MoveRight(float value);

	void BeginCrouch();
	void EndCrouch();

public:

	UFUNCTION(BlueprintCallable,Category="Player")
	void StartFire();
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	void ReloadWeapon();
protected:
	

	void BeginZoom();
	void EndZoom();
	/*
	*Functions
	*/

	//Spawns a weapon and attaches it to the WeaponSocket
	void WeaponSpawn();

	UFUNCTION()
	void OnHealthChange(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
};
