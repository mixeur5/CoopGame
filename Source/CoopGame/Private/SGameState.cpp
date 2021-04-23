// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameState.h"
#include "Net/UnrealNetwork.h"

void ASGameState::OnRep_WaveState(WaveState OldWaveState)
{
	EventWaveStateChanged(CurrentWaveState, OldWaveState);
}

void ASGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASGameState, CurrentWaveState);
}

void ASGameState::ServerSetState(WaveState NewState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		OnRep_WaveState(CurrentWaveState);
		CurrentWaveState = NewState;
	}
}
