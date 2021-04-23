// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUpActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerUpActor::ASPowerUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsActivated = false;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPowerUpActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASPowerUpActor::OnTickPowerup()
{
	TicksProcessed++;
	OnPowerUpTicked();
	if (TicksProcessed >= TotalNumberOfTicks)
	{
		OnExpired();
		GetWorldTimerManager().ClearTimer(TimerHandle_Powerup);
	}
}

void ASPowerUpActor::OnRep_Activated()
{
	OnStateChanged(bIsActivated);
}

void ASPowerUpActor::ActivatePowerup(AActor* TargetActor)
{
	bIsActivated = true;
	
	OnRep_Activated();
	OnActivated(TargetActor);
	if (TickInterval >= 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Powerup, this, &ASPowerUpActor::OnTickPowerup, TickInterval, true);
	}
	else
	{
		OnTickPowerup();
	}
}

void ASPowerUpActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASPowerUpActor, bIsActivated);
}

