// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/SCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "SHealthComponent.h"
#include "..\Public\SCharacter.h"
#include "TimerManager.h"

#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Engine/World.h"
#include "Public/SWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

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
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Weapon, ECR_Ignore);
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	// Array of sockets on which are spawned weapons
	TArray<FName> WeaponSocketNames;
	WeaponStorageSocketNames = { FName(TEXT("spine_socket_1")) , FName(TEXT("spine_socket_1")), FName(TEXT("spine_socket_2")) };

	wantstozoom = false;
	zoomedFOV = 65.0f;
	interpSpeed = 10.0f;
	bIsReloading = false;
	bIsDead = false;
	bIsReceivingBullet = false;
	CurrentWeaponIndex = 0;
	bCanCarryWeapons = true;
	bIsSwappingWeapon = false;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	bIsDead = false;
	bIsReloading = false;
	Super::BeginPlay();
	defaultFOV = CameraComp->FieldOfView;
	if (GetLocalRole() == ENetRole::ROLE_Authority && bCanCarryWeapons)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        // Some Characters can't equip weapons, so check that first (eg: Dragon)
		for (int i = 0; WeaponList.IsValidIndex(i); i++)
		{
			if (WeaponList[i])
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("in spawn: "));
				WeaponVector.Emplace(GetWorld()->SpawnActor<ASWeapon>(WeaponList[i], FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters));
				WeaponVector[i]->SetOwner(this);
				// Attach weapon to corresponding socket.
				if (i == 0) // First weapon is spawned in character's hands
					WeaponVector[i]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponVector[i]->EquipSocketName);
				else
					WeaponVector[i]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponStorageSocketNames[i]);
				WeaponVector[i]->StorageSocketName = WeaponStorageSocketNames[i];
		    }
		}
		// Equipped weapon is the 1rst one
		Weapon = WeaponVector[0];
	}
	bIsFiring = false;
	HealthComp->HealthChangedEvent.AddDynamic(this, &ASCharacter::ActorTakingDamage);
}

void ASCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector()* value);
}

void ASCharacter::BeginCrouch()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("crouch: "));
	Crouch();
}

void ASCharacter::Endcrouch()
{
	UnCrouch();
}

void ASCharacter::Reload()
{
	if (Weapon->CanReload())
	{
		bIsReloading = true;
		Weapon->Reload();
		//This is a hack to make sure the reloading anim is stopped later, as it is bound to bIsReloading
		FTimerHandle TimerHandle_StopAnimation;
		GetWorldTimerManager().SetTimer(TimerHandle_StopAnimation, this, &ASCharacter::StopReloadAnim, 1.0f, false);
	}    
}

void ASCharacter::StopReloadAnim()
{
	bIsReloading = false;
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

void ASCharacter::PreviousWeapon()
{
	SwitchWeapon(CurrentWeaponIndex - 1);
}

void ASCharacter::NextWeapon()
{
	SwitchWeapon(CurrentWeaponIndex + 1 );
}

void ASCharacter::SwitchWeapon(uint8 WeaponIndex)
{
	if (GetLocalRole() != ENetRole::ROLE_Authority)
	{
		ServerSwitchWeapon(WeaponIndex);
	}
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		bIsSwappingWeapon = true;
	}	
	if (WeaponList.IsValidIndex(WeaponIndex))
	{
		CurrentWeaponIndex = WeaponIndex;
	}
	else
	{
		CurrentWeaponIndex = 0;
	}
}

void ASCharacter::SwitchSockets(const uint8 NewWeaponIndex)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		// Detach equipped weapon
		Weapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		// Detach next weapon
		WeaponVector[NewWeaponIndex]->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		// Equip next weapon
		WeaponVector[NewWeaponIndex]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->EquipSocketName);
		// Attach weapon to inventory socket
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->StorageSocketName);
		Weapon = WeaponVector[NewWeaponIndex];
		bIsSwappingWeapon = false;
	}	
}

void ASCharacter::Client_SwitchWeapon()
{
	SwitchWeapon(CurrentWeaponIndex);
}

void ASCharacter::ServerSwitchWeapon_Implementation(uint8 WeaponIndex)
{
	UE_LOG(LogTemp, Log, TEXT("Server switch weapon!!"));
	SwitchWeapon(WeaponIndex);
}

bool ASCharacter::ServerSwitchWeapon_Validate(uint8 WeaponIndex)
{
	return true;
}

void ASCharacter::EnableRagdoll()
{
	GetMesh()->SetSimulatePhysics(true);
	//GetWorldTimerManager().ClearTimer(RagdollTimerHandler);
}

void ASCharacter::StartFire()
{
	if (Weapon)
	{
		bIsFiring = true;
		Weapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (Weapon)
	{
		bIsFiring = false;
		Weapon->StopFire();
	}
}

void ASCharacter::ActorTakingDamage(USHealthComponent* PassedHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (PassedHealthComp->CurrentHealth <= 0 && !bIsDead)
	{
		//GetWorldTimerManager().SetTimer(
		//	RagdollTimerHandler, this, &ASCharacter::EnableRagdoll, 1.0f, false);
		EnableRagdoll();
		bIsDead = true;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();
		SetLifeSpan(5.0f);
	}
}
void ASCharacter::RefillAmmo()
{
	Weapon->RefillAmmo();
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
	PlayerInputComponent->BindAction("PreviousWeapon", IE_Pressed, this, &ASCharacter::PreviousWeapon);
	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &ASCharacter::NextWeapon);
	PlayerInputComponent->BindAction("BeginZoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("EndZoom", IE_Released, this, &ASCharacter::EndZoom);
	PlayerInputComponent->BindAction("StartFire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("StopFire", IE_Released, this, &ASCharacter::StopFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::Reload);
}

void ASCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASCharacter, Weapon);
	DOREPLIFETIME(ASCharacter, bIsDead);
	DOREPLIFETIME(ASCharacter, WeaponVector);
	DOREPLIFETIME(ASCharacter, bIsSwappingWeapon);
	DOREPLIFETIME_CONDITION(ASCharacter, bIsFiring, COND_SkipOwner);
	//DOREPLIFETIME(ASCharacter, CurrentWeaponIndex);
	DOREPLIFETIME_CONDITION(ASCharacter, wantstozoom, COND_SkipOwner);
}

