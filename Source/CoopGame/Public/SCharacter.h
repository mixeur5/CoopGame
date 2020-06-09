// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class ASWeapon;

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

	bool wantstozoom;

	float defaultFOV;

	UPROPERTY(EditDefaultsOnly, Category ="Player")
	float zoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float interpSpeed;

	void BeginZoom();

	void EndZoom();

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> bWeaponClass;

	ASWeapon* Weapon;

	void StartFire();

	void StopFire();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
