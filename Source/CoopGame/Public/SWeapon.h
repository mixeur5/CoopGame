// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraShake.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "SWeapon.generated.h"


#define ECC_Weapon ECC_GameTraceChannel1

class UParticleSystem;

USTRUCT()
struct FHitScanStruct
{
	GENERATED_BODY()
public :

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> PhysicalSurfaceElement;

	UPROPERTY()
	FVector_NetQuantize LineTraceEndElement;
};

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
	UStaticMeshComponent* StaticMeshComp;
	
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

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		UParticleSystem* RockImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		UParticleSystem* WaterImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		UParticleSystem* ExplodeImpactEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	FName socketname;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float BulletsPerMin;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		TSubclassOf<UMatineeCameraShake> CameraShakeWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (ClampMin=0.0f))
    float BulletSpreadDegrees;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (ClampMin = 0.0f))
	float WeaponDamage;
	
	float SecondsBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	uint8 MaxBackupBullets;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	uint8 MaxMagazineCapacity;

	float PreviousFireTime;

	void PlayEffects(FVector TraceEnd);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
    virtual void Fire();

	UPROPERTY(ReplicatedUsing = Client_FireEffects)
	FHitScanStruct HitScanStruct;

	UFUNCTION()
	void Client_FireEffects();

	void PlayFireImpact(EPhysicalSurface, FVector TraceEnd);

	FTimerHandle TimerHandler;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	uint8 BackupBulletsRemaining;

	UPROPERTY(BlueprintReadOnly)
	uint8 MagazineBulletsRemaining;

	void Reload();

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bIsFiring;

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bIsReloading;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopFire();

	void RefillAmmo();

	bool CanReload();

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	FName EquipSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	FName StorageSocketName;
};
