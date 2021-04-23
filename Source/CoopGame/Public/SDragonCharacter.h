// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCharacter.h"
#include "SProjectile.h"
#include "SDragonCharacter.generated.h"

/**
 * 
 */

UCLASS()
class COOPGAME_API ASDragonCharacter : public ASCharacter
{
	GENERATED_BODY()

public:

	ASDragonCharacter();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	FName socketname;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float BulletsPerMin;

	float SecondsBetweenShots;

	float PreviousFireTime;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
		UParticleSystem* FireParticleSystem;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<ASProjectile> ProjectileClass;

	void StartFire() override;

	void Fire();

	void StopFire() override;

	FTimerHandle TimerHandler;

	UPROPERTY(BlueprintReadWrite)
	bool bWantsToFly;
};
