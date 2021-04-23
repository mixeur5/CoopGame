// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChanged, USHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void TakingDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser );

public:	
	// Called every frame

	UPROPERTY(Replicated, BlueprintReadOnly)
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		uint8 TeamNumber;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float DefaultHealth;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnHealthChanged HealthChangedEvent;

	float GetHealth()
	{
		return CurrentHealth;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay")
	static bool IsFriendly(AActor* ActorA, AActor* ActorB);
	
};
