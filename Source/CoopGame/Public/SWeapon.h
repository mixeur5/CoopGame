// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"


class UParticleSystem;
UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SkelMeshComp;
	

	UPROPERTY(VisibleAnywhere, Category = "Gameplay")
	TSubclassOf<UDamageType> damagetype;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* muzzleflash;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* smokeparticle;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	FName socketname;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float BulletsPerMin;
	
	float SecondsBetweenShots;

	float PreviousFireTime;

	void PlayEffects(FVector TraceEnd);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void Fire();

	FTimerHandle TimerHandler;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopFire();
};
