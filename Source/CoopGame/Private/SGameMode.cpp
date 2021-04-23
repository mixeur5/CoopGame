// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "SHealthComponent.h"
#include "SGameState.h"
#include "EngineGlobals.h"
#include "EngineUtils.h"


ASGameMode::ASGameMode()
{
	bTotalNumberofSpawns = 3;
	PrimaryActorTick.bCanEverTick = true ;
	GameStateClass = ASGameState::StaticClass();
	PrimaryActorTick.TickInterval = 2.0f;
}

void ASGameMode::StartPlay()
{
	bTotalNumberofSpawns = 3;
	Super::StartPlay();
	PrepareNextWave();
	
}

void ASGameMode::RespawnDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		APawn* PlayerPawn = PlayerController->GetPawn();
		if (PlayerController && PlayerPawn == nullptr)
		{
			RestartPlayer(PlayerController);
		}
	}
}

void ASGameMode::CheckWaveState()
{

	if (GetWorldTimerManager().IsTimerActive(TimerHandle_PrepareSpawns) || bRemainingNumberofSpawns != 0)
	{
		return;
	}
	bool bBotsAreAlive= false;
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* TestPawn = *It;
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}
		USHealthComponent* HealthComponent = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (ensureAlways(HealthComponent) && HealthComponent->GetHealth() > 0.0f)
		{
			bBotsAreAlive = true;
			break;
		}
	}
	if (!bBotsAreAlive)
	{
		PrepareNextWave();
	}
}

void ASGameMode::Tick(float tick)
{
	Super::Tick(tick);
	CheckWaveState();
	CheckPlayersAlive();

}

void ASGameMode::PrepareNextWave()
{
	SetGameState(WaveState::PrepareForNextWave);
	RespawnDeadPlayers();
	UE_LOG(LogTemp, Warning, TEXT("Prepare Next Wave"));
	GetWorldTimerManager().SetTimer(TimerHandle_PrepareSpawns, this, &ASGameMode::StartWave, 2.0f, false);
}

void ASGameMode::StartWave()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Wave"));
	bTotalNumberofSpawns = bTotalNumberofSpawns*2;
	bRemainingNumberofSpawns = bTotalNumberofSpawns;
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawns, this, &ASGameMode::LoopBotSpawns, 4.0f, true, 1.0f);
}

void ASGameMode::EndWave()
{
	SetGameState(WaveState::KillingInProgress);
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawns);
}

void ASGameMode::LoopBotSpawns()
{
	SetGameState(WaveState::SpawningInProgress);
	UE_LOG(LogTemp, Warning, TEXT("Loop Spawns"));
	bRemainingNumberofSpawns--;
	// This function is blueprint-implemented
	SpawnNewBot();
	if (bRemainingNumberofSpawns == 0)
	{
		EndWave();
	}
}

void ASGameMode::SetGameState(WaveState NewWaveState)
{
	ASGameState* GameStateMember=GetGameState<ASGameState>();
	GameStateMember->CurrentWaveState = NewWaveState;
}


void ASGameMode::CheckPlayersAlive()
{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
		{
			APlayerController* PlayerController = It->Get();
			APawn* PlayerPawn = PlayerController->GetPawn();
			if (PlayerPawn && PlayerController)
			{
				USHealthComponent* HealthComp = Cast<USHealthComponent>(PlayerPawn->GetComponentByClass(USHealthComponent::StaticClass()));
				if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
				{
					return;
				}
			}
		}
		GameOver();
}

void ASGameMode::GameOver()
{
	SetGameState(WaveState::GameOver);
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawns);
}
