// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "SCharacter.generated.h"



class USpringArmComponent;
class UCameraComponent;
class ASWeapon;
class USHealthComponent;


UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveRight(float value);

	void MoveForward(float value);

	void BeginCrouch();

	void Endcrouch();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringComp;

	void Reload();

	void StopReloadAnim();

	float defaultFOV;

	UPROPERTY(EditDefaultsOnly, Category ="Player")
	float zoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float interpSpeed;

	void BeginZoom();

	void EndZoom();

	void PreviousWeapon();

	void NextWeapon(); 

	void SwitchWeapon(uint8 WeaponIndex);

	// Array of sockets on which are spawned weapons
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TArray<FName> WeaponStorageSocketNames;

	void EnableRagdoll();

	UPROPERTY(Replicated, VisibleAnyWhere, BlueprintReadOnly, Category = "Gameplay")
	USHealthComponent* HealthComp;

	UPROPERTY(Replicated, BlueprintReadOnly)
	ASWeapon* Weapon;
    
	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<ASWeapon*> WeaponVector;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TArray<TSubclassOf<ASWeapon>> WeaponList;

	UFUNCTION()
	void ActorTakingDamage(USHealthComponent* HealthCompParam, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void Client_SwitchWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSwitchWeapon(uint8 WeaponIndex);

public:	

	void SwitchSockets(const uint8 NewWeaponIndex);

	uint8 CurrentWeaponIndex;

	// Tag that lets the animation system know when to fire the swapping animation
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bIsSwappingWeapon;

	void RefillAmmo();

	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bIsDead;

	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bIsFiring;

	UPROPERTY(Replicated, BlueprintReadWrite)
	bool wantstozoom;

	UPROPERTY(Replicated, BlueprintReadWrite)
		bool bIsReloading;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category ="Player")
	virtual void StartFire();

	UFUNCTION(BlueprintCallable, Category="Player")
	virtual void StopFire();
	
	bool bIsReceivingBullet;

	bool bCanCarryWeapons;

	UFUNCTION(BlueprintImplementableEvent, Category ="Game")
	void BulletReceived(FName BoneImpactName);

};
