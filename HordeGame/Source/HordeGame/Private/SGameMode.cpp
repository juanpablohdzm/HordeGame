// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include <TimerManager.h>
#include "MyComponents/SHealthComponent.h"
#include "SGameState.h"
#include "SPlayerState.h"





ASGameMode::ASGameMode()
{
	TimeBetweenWaves = 2.0f;

	GameStateClass = ASGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}



void ASGameMode::StartPlay()
{
	Super::StartPlay();

}

void ASGameMode::ReadyToPlay()
{
	PrepareForNextWave();
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
	CheckAnyPlayerAlive();
}

void ASGameMode::PrepareForNextWave()
{
	RestartDeadPlayers();
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBetweenWaves, false);
	SetWaveState(EWaveState::WaitingToStart);
}



void ASGameMode::StartWave()
{
	WaveCount++;
	NrOfBotsToSpawn = 2 * WaveCount;
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner,this,&ASGameMode::SpawnBotTimerElapsed,1.0f,true,0.0f);
	SetWaveState(EWaveState::WaveInProgress);
}

void ASGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;
	if (NrOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
	SetWaveState(EWaveState::WaitingToComplete);
}

void ASGameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NrOfBotsToSpawn > 0 || bIsPreparingForWave)
	{
		return;
	}
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It;++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled()) { continue; }
		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetCurrentHealth() > 0)
		{
			return;
		}
	}
	SetWaveState(EWaveState::WaveComplete);
	PrepareForNextWave();
}

void ASGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It=GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController * PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn * Pawn = PC->GetPawn();
			USHealthComponent*HealthComp=Cast<USHealthComponent>(Pawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if (ensure(HealthComp)&& HealthComp->GetCurrentHealth()>0.0f) // this breaks the code in case is null
			{
				// A player is still alive
				return;
			}
		}
	}

	// No player alive
	GameOver();
	
}

void ASGameMode::GameOver()
{
	EndWave();
	SetWaveState(EWaveState::GameOver);
}

void ASGameMode::SetWaveState(EWaveState NewState)
{
	ASGameState * GS = GetGameState<ASGameState>();
	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);
	}
}

void ASGameMode::RestartDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController * PC = It->Get();
		if (PC && PC->GetPawn()== nullptr)
		{
			RestartPlayer(PC);
		}
	}
}

