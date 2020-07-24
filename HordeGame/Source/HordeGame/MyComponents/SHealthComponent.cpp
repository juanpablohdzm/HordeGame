// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"
#include <Net/UnrealNetwork.h>
#include <GameFramework/Actor.h>
#include "SGameMode.h"


// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100.0f;
	bIsDead = false;

	SetTeamNum(255);

}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	SetIsReplicated(true);
	Super::BeginPlay();

	// ... Begin play is only called on the server, technically it is not necessary to use the next if
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor * MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}
	
	
	CurrentHealth = DefaultHealth;
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//This is only done on the server because OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage); only happens in the server 
	if (Damage <= 0.0f || bIsDead) return;
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, DefaultHealth);

	if (DamageCauser != DamagedActor && IsFriendly(GetOwner(), DamageCauser)) { return; } // They are buddies...

	bIsDead = CurrentHealth <= 0.0f;

	HealthInfo.Health = CurrentHealth;
	HealthInfo.HealthDelta = Damage;
	HealthInfo.DamageType = DamageType;
	HealthInfo.Instigator = InstigatedBy;
	HealthInfo.DamageCauser = DamageCauser;

	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)
	{
		ASGameMode * GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser,InstigatedBy);
		}
	}
}


void USHealthComponent::OnRep_HealthInfo()
{
	OnHealthChanged.Broadcast(this, HealthInfo.Health, HealthInfo.HealthDelta,HealthInfo.DamageType,HealthInfo.Instigator,HealthInfo.DamageCauser);
}

void USHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <=0.0f || CurrentHealth <=0.0f)
	{
		return;
	}
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, DefaultHealth);
	OnHealthChanged.Broadcast(this, CurrentHealth, HealAmount, nullptr, nullptr, nullptr);

}

bool USHealthComponent::IsFriendly(AActor * ActorA, AActor* ActorB)
{
	if (ActorA ==nullptr || ActorB == nullptr)
	{
		//Assume friendly
		return true;
	}

	USHealthComponent * HealthCompA = Cast<USHealthComponent>(ActorA->GetComponentByClass(USHealthComponent::StaticClass()));
	USHealthComponent * HealthCompB = Cast<USHealthComponent>(ActorB->GetComponentByClass(USHealthComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		//Assume friendly
		return true;
	}

	return HealthCompA->GetTeamNum() == HealthCompB->GetTeamNum();

}

void USHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, CurrentHealth);
	DOREPLIFETIME(USHealthComponent, HealthInfo);
}

