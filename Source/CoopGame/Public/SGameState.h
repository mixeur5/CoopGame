// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

/**
 * 
 */
UENUM()
enum class WaveState :uint8
{
	PrepareForNextWave,
	SpawningInProgress,
	KillingInProgress,
	GameOver
};

UCLASS()
class COOPGAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
protected:

	UFUNCTION()
		void OnRep_WaveState(WaveState OldWaveState);

	UFUNCTION(BlueprintImplementableEvent, Category = "Gamestate")
		void EventWaveStateChanged(WaveState OldWaveState, WaveState NewWaveState);
public:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
		WaveState CurrentWaveState;

	void ServerSetState(WaveState NewState);
	
};
