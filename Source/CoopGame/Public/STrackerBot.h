// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SHealthComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Particles/ParticleSystem.h"
#include "STrackerBot.generated.h"


UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector FindNextPathPoint();

	bool bIsStarting;

	UPROPERTY(BlueprintReadOnly)
	FVector NextPathPoint;

	USHealthComponent* HealthComponent;

	UFUNCTION()
		void ActorTakingDamage(USHealthComponent* HealthCompParam, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void BotExplosion();

	void Client_BotExplosion();

	UPROPERTY(VisibleDefaultsOnly, Category ="Components")
	UStaticMeshComponent* MeshComp;

	bool bIsDestroyed;

	URadialForceComponent* RadialForce;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* ExplodingParticleSystem;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
