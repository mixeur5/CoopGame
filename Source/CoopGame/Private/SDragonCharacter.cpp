// Fill out your copyright notice in the Description page of Project Settings.

#include "SDragonCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


ASDragonCharacter::ASDragonCharacter()
{
	BulletsPerMin = 20.0f;
	PreviousFireTime = 0.0f;
	bIsFiring = false;
	bCanCarryWeapons = false;
	socketname = "dragon_tongue_tip";
}


void ASDragonCharacter::BeginPlay()
{
	Super::BeginPlay();
	bIsFiring = false;
	if (BulletsPerMin)
	{
		SecondsBetweenShots = 60 / BulletsPerMin;
	}
	PreviousFireTime = 0.0f;
}

void ASDragonCharacter::StartFire()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("startfire "));
	if (GetLocalRole() != ENetRole::ROLE_Authority)
		return;
	bIsFiring = true;
	float FireDelay = FMath::Max(SecondsBetweenShots - (GetWorld()->TimeSeconds - PreviousFireTime), 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandler, this, &ASDragonCharacter::Fire, SecondsBetweenShots, true, FireDelay);
}

void ASDragonCharacter::Fire()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("fire "));
		
		FVector eyesvector;
		FRotator eyesrotation;
		this->GetActorEyesViewPoint(eyesvector, eyesrotation);
		FVector shotdirection = eyesrotation.Vector();
		FVector eyesend = eyesvector + shotdirection * 10000;

		FVector MuzzleLocation = this->GetMesh()->GetSocketLocation(socketname);
		FRotator MuzzleRotation = this->GetMesh()->GetSocketRotation(socketname);
			
			//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Instigator = GetOwner()->GetInstigator();
			// spawn the projectile at the muzzle
		if (ProjectileClass)
		{
			UGameplayStatics::SpawnEmitterAttached(FireParticleSystem, GetMesh(), socketname);
			GetWorld()->SpawnActor<ASProjectile>(ProjectileClass, MuzzleLocation, shotdirection.Rotation(), ActorSpawnParams);
		}
	
	PreviousFireTime = GetWorld()->TimeSeconds;
}

void ASDragonCharacter::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandler);
	bIsFiring = false;
}
