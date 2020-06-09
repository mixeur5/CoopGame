// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/SCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "..\Public\SCharacter.h"


#include "Components/InputComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Engine/World.h"
#include "Public/SWeapon.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));

	SpringComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringComp"));
	SpringComp->bUsePawnControlRotation = true;
	SpringComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(SpringComp);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	wantstozoom = false;
	zoomedFOV = 65.0f;
	interpSpeed = 10.0f;

}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	defaultFOV = CameraComp->FieldOfView;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if(bWeaponClass)
	{
		Weapon = GetWorld()->SpawnActor<ASWeapon>(bWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
		Weapon->SetOwner(this);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("hand_rSocket")));
	}
	
}

void ASCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector()* value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::Endcrouch()
{
	UnCrouch();
}

void ASCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector()* value);
}

void ASCharacter::BeginZoom()
{
	wantstozoom = true;
}

void ASCharacter::EndZoom()
{
	wantstozoom = false;
}

void ASCharacter::StartFire()
{
	if (Weapon)
	{
		Weapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (Weapon)
	{
		Weapon->StopFire();
	}
}
// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float targetFOV = wantstozoom ? zoomedFOV : defaultFOV;
	float newFOV = FMath::FInterpTo(CameraComp->FieldOfView, targetFOV, 0.7f,interpSpeed);
	CameraComp->SetFieldOfView(newFOV);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookSideways",this, &ASCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("StopJump", IE_Released, this, &ASCharacter::StopJumping);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);

	PlayerInputComponent->BindAction("UnCrouch", IE_Released, this, &ASCharacter::Endcrouch);
	PlayerInputComponent->BindAction("BeginZoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("EndZoom", IE_Released, this, &ASCharacter::EndZoom);
	PlayerInputComponent->BindAction("StartFire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("StopFire", IE_Released, this, &ASCharacter::StopFire);	
}

