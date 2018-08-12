// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

UCLASS()
class HORDEGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:
	/*
	Components
	*/
	UPROPERTY(VisibleAnywhere,Category="Components")
	class USHealthComponent * HealthComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent * MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class URadialForceComponent * RadialForceComp;


	/*
	Functions
	*/

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnHealthChange(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnRep_Exploded();

	/*
	Variables
	*/
	UPROPERTY(ReplicatedUsing=OnRep_Exploded)
	bool bExploded=false;

	UPROPERTY(EditDefaultsOnly,Category="FX")
	float ExplosionImpulse;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage;
	
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	class UParticleSystem * ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	class UMaterialInstance * ExplodedMaterial;







	
	
};
