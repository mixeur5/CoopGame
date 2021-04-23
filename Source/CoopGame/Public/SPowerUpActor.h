// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerUpActor.generated.h"

UCLASS()
class COOPGAME_API ASPowerUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerUpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	int32 TotalNumberOfTicks;

	int32 TicksProcessed;

	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	float TickInterval;

	FTimerHandle TimerHandle_Powerup;

	UFUNCTION()
		void OnTickPowerup();

	UPROPERTY(ReplicatedUsing = OnRep_Activated)
		bool bIsActivated;

	UFUNCTION()
		void OnRep_Activated();

public:	

	void ActivatePowerup(AActor* TargetActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
		void OnStateChanged(bool bIsActivated);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
		void OnActivated(AActor* TargetActor);

		UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
		void OnExpired();

		UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
		void OnPowerUpTicked();
};
