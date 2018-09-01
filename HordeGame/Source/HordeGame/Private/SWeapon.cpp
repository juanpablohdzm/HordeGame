// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include <Components/SkeletalMeshComponent.h>
#include <DrawDebugHelpers.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>
#include <PhysicalMaterials/PhysicalMaterial.h>
#include <TimerManager.h>
#include "HordeGame.h"
#include <Net/UnrealNetwork.h>
#include "SCharacter.h"


static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines for Weapons"), ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Comp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TraceTargetName = "BeamEnd";
	BaseDamage = 20.0f;
	RateOfFire = 600.0f;
	MaxAmmo = 100;
	MaxRound = 10;
	CurrentAmmo = 0;
	CurrentRound = 0;
	BulletSpreadAngle = 2.0f;

	
	SetReplicates(true);
}


void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	TimeBetweenShots = 60 / RateOfFire;
}

void ASWeapon::Fire()
{
	//Trace the world, from pawn eyes to crosshair location
	if (Role < ROLE_Authority)
	{
		ASCharacter * MyOwner = Cast<ASCharacter>(GetOwner());
		MyOwner->BulletCount++;

		ServerFire();
		return;
	}

	if (CurrentAmmo >= MaxAmmo || CurrentRound >= MaxRound) return;

	ASCharacter * MyOwner = Cast<ASCharacter>(GetOwner());
	if (MyOwner)
	{
		
		MyOwner->BulletCount++;
		FVector EyesLocation;
		FRotator EyesRotation;
		MyOwner->GetActorEyesViewPoint(EyesLocation,EyesRotation);

		FVector ShotDirection = EyesRotation.Vector();

		//Bullet spread, random
		float HalfRad = FMath::DegreesToRadians(BulletSpreadAngle);
		ShotDirection=FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceEnd = EyesLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FHitResult Hit;
		EPhysicalSurface SurfaceType=SurfaceType_Default;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyesLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			//Blocking hit! Process damage

			TraceEnd = Hit.ImpactPoint;
			AActor * HitActor = Hit.GetActor();
			
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			float ActualDamage = BaseDamage;
			
			if (SurfaceType== SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

			//This function is called in case the server is also a host 
			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			
		}

		if(DebugWeaponDrawing>0)
			DrawDebugLine(GetWorld(), EyesLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);

		//This function is called in case the server is also a host 
		PlayFireEffects(TraceEnd);

		if (Role == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TraceEnd;
			HitScanTrace.SurfaceType = SurfaceType;
		}

		LastFireTime = GetWorld()->TimeSeconds;

		CurrentAmmo++;
		CurrentRound++;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Miss owner in weapon, can´t shoot"));
	}
}

void ASWeapon::OnRep_HitScanTrace()
{
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASWeapon::PlayFireEffects(FVector TraceEnd)
{
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, MeshComp, MuzzleSocketName);
	}

	if (SmokeTrail)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent * ParticleSysComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeTrail, MuzzleLocation);
		if (ParticleSysComp)
		{
			ParticleSysComp->SetVectorParameter(TraceTargetName, TraceEnd);
		}
	}

	APawn * MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController * PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}

}

void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem * SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation(), true);
	}
}

void ASWeapon::ServerFire_Implementation()
{
	Fire();
	
}

bool ASWeapon::ServerFire_Validate()
{
	return true;
}

void ASWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds,0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots,this,&ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}


void ASWeapon::ServerStopFire_Implementation()
{
	CurrentRound = 0;
}

bool ASWeapon::ServerStopFire_Validate()
{
	return true;
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
	ServerStopFire();

}

void ASWeapon::ServerReload_Implementation()
{
	CurrentAmmo = 0;
}

bool ASWeapon::ServerReload_Validate()
{
	return true;
}

void ASWeapon::Reload()
{
	ServerReload();
}

void ASWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASWeapon, CurrentAmmo);
	DOREPLIFETIME(ASWeapon, CurrentRound);
	DOREPLIFETIME(ASWeapon, HitScanTrace);
}


