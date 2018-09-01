// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"


UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaveComplete,
	WaitingToStart,
	PreparingNextWave, 
	WaveInProgress,
	//No longer spawning bots, waiting for players to kill remaining bots
	WaitingToComplete,
	GameOver,
};

/**
 * 
 */

UCLASS()
class HORDEGAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(Replicated,BlueprintReadOnly,Category="Game State")
	int CurrentWaveNum=0;

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);

	EWaveState GetWaveState() const { return WaveState; }
	void SetWaveState(EWaveState val) { if (Role == ROLE_Authority) { OnRep_WaveState(WaveState); WaveState = val; } }
protected:
	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
	EWaveState WaveState=EWaveState::WaitingToStart;
	
	
	
};
