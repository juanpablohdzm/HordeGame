// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

USTRUCT()
struct FHealthInfo
{
	GENERATED_BODY()
public:	
	UPROPERTY()
	float Health;
	
	UPROPERTY()
	float HealthDelta;
	
	UPROPERTY()
	const UDamageType * DamageType;
	
	UPROPERTY()
	AController * Instigator;
	
	UPROPERTY()
	AActor * DamageCauser;

};

//On Health change event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Horde), meta=(BlueprintSpawnableComponent) )
class HORDEGAME_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	//Variables

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	float DefaultHealth;

	UPROPERTY(Replicated,EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	float CurrentHealth;

	UPROPERTY(ReplicatedUsing=OnRep_HealthInfo)
	FHealthInfo HealthInfo;

	bool bIsDead;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="HealthComponent")
	uint8 TeamNum;

	//Functions
	UFUNCTION()
	virtual void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	virtual void OnRep_HealthInfo();

public:
	UFUNCTION(BlueprintCallable)
	virtual void Heal(float HealAmount);

	float GetCurrentHealth() const { return CurrentHealth; }
	void SetCurrentHealth(float val) { CurrentHealth = val; }

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="HealthComponent")
	static bool IsFriendly(AActor * ActorA,AActor* ActorB);

	uint8 GetTeamNum() const { return TeamNum; }
	void SetTeamNum(uint8 val) { TeamNum = val; }
public:

	UPROPERTY(BlueprintAssignable,Category="Event")
	FOnHealthChangedSignature OnHealthChanged;
	
};
