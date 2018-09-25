// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "SWeapon.h"
#include "HordeGame.h"
#include "MyComponents/SHealthComponent.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/PawnMovementComponent.h>
#include <Components/CapsuleComponent.h>
#include <Engine/World.h>
#include <Net/UnrealNetwork.h>



// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 250.0f;
	SpringArmComp->AddRelativeLocation(FVector(0, 0, 50.0f));
	SpringArmComp->bUsePawnControlRotation = true;

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));




	ZoomFOV = 65.0f;
	ZoomInterpSpeed = 20.0f;
	bDied = false;

}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if(Role ==ROLE_Authority)
		WeaponSpawn();

	DefaultFOV=CameraComp->FieldOfView;

	
}


// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);

	
}



void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChange);
}

void ASCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Weapon->Destroy();
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
	
}



void ASCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector()* value);
}

void ASCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector()*value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::StartFire()
{
	if (Weapon)
	{
		Weapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (Weapon)
	{
		Weapon->StopFire();
	}
}

void ASCharacter::ReloadWeapon()
{
	if (Weapon)
	{
		BulletCount = 0;
		Weapon->Reload();
	}
}

void ASCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}



// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed,this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released,this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

	PlayerInputComponent->BindAction("Reload", IE_Released, this, &ASCharacter::ReloadWeapon);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);
}

void ASCharacter::WeaponSpawn()
{

	//This is called only on the server ROLE_Authority
	if (RifleClass)
	{


		const FVector SocketLocation = GetMesh()->GetSocketLocation("WeaponSocket");
		const FRotator SocketRotation = GetMesh()->GetSocketRotation("WeaponSocket");

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.Instigator = this;
		SpawnParameters.Owner = this;

		//Spawn Weapon
		Weapon = GetWorld()->SpawnActor<ASWeapon>(RifleClass, SocketLocation, SocketRotation, SpawnParameters);

		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, true);

		if(Weapon)
			Weapon->AttachToComponent(GetMesh(), AttachmentTransformRules, FName(TEXT("WeaponSocket")));


	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Missing Weapon class"));
	}
}

void ASCharacter::OnHealthChange(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Role == ROLE_Authority)
	{
		if (Health <= 0.0f && !bDied)
		{
			//Die!
			bDied = true;

			GetMovementComponent()->StopMovementImmediately();
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			DetachFromControllerPendingDestroy();

			SetLifeSpan(5.0f);

		}
	}
}



void ASCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASCharacter, Weapon);
	DOREPLIFETIME(ASCharacter, bDied);

	//Rotation is being replicated in parent´s class, but if you want to show in animation BP you have to use GetBaseAimRotation 
}



