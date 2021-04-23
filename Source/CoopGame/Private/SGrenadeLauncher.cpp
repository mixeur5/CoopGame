// Fill out your copyright notice in the Description page of Project Settings.


#include "SGrenadeLauncher.h"
#include "SCharacter.h"
#include "SProjectile.h"

ASGrenadeLauncher::ASGrenadeLauncher()
{
	BackupBulletsRemaining = 12;
	MaxMagazineCapacity = 6;
	MagazineBulletsRemaining = MaxMagazineCapacity;
}

 void ASGrenadeLauncher::Fire() 
{
	 GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("FIRE: "));
	// Don't fire if no more bullets in charger
	if (MagazineBulletsRemaining == 0)
		return;
	if (GetLocalRole() != ENetRole::ROLE_Authority)
	{
		// When this code triggers on a client, send RPC to server so he knows a client is firing
		ServerFire();
	}

	AActor* owningactor = GetOwner();
	ASCharacter* FiringCharacter = Cast<ASCharacter>(owningactor);
	// Prevent from firing if owner is dead/destroyed
	if (owningactor && !FiringCharacter->bIsDead)
	{
		bIsFiring = true;
		FVector eyesvector;
		FRotator eyesrotation;
		owningactor->GetActorEyesViewPoint(eyesvector, eyesrotation);
		FVector shotdirection = eyesrotation.Vector();
		FVector eyesend = eyesvector + shotdirection * 10000;

		float angle_rad = FMath::DegreesToRadians(BulletSpreadDegrees);
		FMath::VRandCone(eyesend, angle_rad);

		FHitResult hitresult;
			if (GetLocalRole() == ENetRole::ROLE_Authority)
			{
				FVector MuzzleLocation = StaticMeshComp->GetSocketLocation(socketname);
				FRotator MuzzleRotation = StaticMeshComp->GetSocketRotation(socketname);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				ActorSpawnParams.Instigator = GetOwner()->GetInstigator();
				// spawn the projectile at the muzzle
				if (ProjectileClass)
				{
					ASProjectile* Projectile = GetWorld()->SpawnActor<ASProjectile>(ProjectileClass, MuzzleLocation, shotdirection.Rotation(), ActorSpawnParams);
					Projectile->SetReplicates(true);
				}
			}
			PreviousFireTime = GetWorld()->TimeSeconds;
			Cast<ASCharacter>(GetOwner())->bIsFiring = false;
			MagazineBulletsRemaining -= 1;
	}
}

