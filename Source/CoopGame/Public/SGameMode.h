// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

#ifndef CHECK_PUREVIRTUALS
#define CHECK_PUREVIRTUALS 1
#endif

enum class WaveState :uint8;
/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected :
	ASGameMode();

	FTimerHandle TimerHandle_BotSpawns;

	FTimerHandle TimerHandle_PrepareSpawns;

	int32 bTotalNumberofSpawns;

	int32 bRemainingNumberofSpawns;

	void StartWave();

	void EndWave();

	void PrepareNextWave();

	void LoopBotSpawns();

	void SetGameState(WaveState NewWaveState);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game")
	void SpawnNewBot();

	virtual void StartPlay() override;

	void RespawnDeadPlayers();

	void CheckWaveState();

	void CheckPlayersAlive();

	void GameOver();

	virtual void Tick(float tick) override;
};
