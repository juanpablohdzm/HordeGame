// Fill out your copyright notice in the Description page of Project Settings.

#include "SGrenadeLauncher.h"
#include <Engine/World.h>
#include <Components/SkeletalMeshComponent.h>
#include <Kismet/GameplayStatics.h>




void ASGrenadeLauncher::Fire()
{
	if (ProjectileClass)
	{
		const FVector  MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		const FRotator  MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParameters);
	}
}
