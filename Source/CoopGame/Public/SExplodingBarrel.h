// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplodingBarrel.generated.h"
class USHealthComponent;
class URadialForceComponent;
class UParticleSystem;

UCLASS()
class COOPGAME_API ASExplodingBarrel : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = Client_BarrelExplosion)
	bool bIsDestroyed;

	UPROPERTY(VisibleAnyWhere, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UMaterialInterface* MaterialInterface;

	URadialForceComponent* RadialForce;

	UPROPERTY(VisibleAnyWhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* ExplodingParticleSystem;

	UFUNCTION()
	void ActorTakingDamage(USHealthComponent* HealthCompParam, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void Client_BarrelExplosion();


	void BarrelExplosion();
	
public:	
	// Sets default values for this actor's properties
	ASExplodingBarrel();

};
