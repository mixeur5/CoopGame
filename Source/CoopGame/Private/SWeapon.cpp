// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\SWeapon.h"
#include "DrawDebugHelpers.h"
#include "PhysicsInterfacePhysX.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVarDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw debug lines"), ECVF_Cheat);
// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SkelMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StaticMeshComp"));
	socketname = "MuzzleFlash";
	BulletsPerMin = 300.0f;
	PreviousFireTime = 0.0f;
	
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	if(BulletsPerMin)
	{
		SecondsBetweenShots = 60 / BulletsPerMin;
	}
}

void ASWeapon::PlayEffects(FVector eyesend)
{
	if (smokeparticle)
	{
		FVector muzzlelocation = SkelMeshComp->GetSocketLocation(socketname);
		UParticleSystemComponent* impactcomp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), smokeparticle, muzzlelocation);
		if (impactcomp)
		{
			impactcomp->SetVectorParameter("Target", eyesend);
		}
	}

    if (muzzleflash)
    {
	    UGameplayStatics::SpawnEmitterAttached(muzzleflash, SkelMeshComp, socketname);
    }
}

void ASWeapon::Fire()
{
	AActor* owningactor = GetOwner();
	if (owningactor)
	{
		FVector eyesvector;
		FRotator eyesrotation;
		owningactor->GetActorEyesViewPoint(eyesvector, eyesrotation);
		FVector shotdirection = eyesrotation.Vector();
		FVector eyesend = eyesvector + shotdirection * 10000;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(owningactor);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;
		FHitResult hitresult;
		if (GetWorld()->LineTraceSingleByChannel(hitresult, eyesvector, eyesend, ECC_Visibility, QueryParams))
		{
			AActor* hitactor = hitresult.GetActor();
			eyesend = hitresult.ImpactPoint;
			UGameplayStatics::ApplyPointDamage(hitactor, 20.0f, shotdirection, hitresult, owningactor->GetInstigatorController(), owningactor, damagetype);

			UParticleSystem* ImpactEffect;
			EPhysicalSurface HitSurface = UPhysicalMaterial::DetermineSurfaceType(hitresult.PhysMaterial.Get());
			switch(HitSurface)
			{
			case SurfaceType1:
				ImpactEffect = FleshImpactEffect;
			case SurfaceType2:
				ImpactEffect = FleshImpactEffect;
				break;
			default:
				ImpactEffect = DefaultImpactEffect;
				break;
			}

			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, hitresult.Location, hitresult.Normal.Rotation());
			}
		}
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), eyesvector, eyesend, FColor::Green, false, 1.0f, 0, 1.0f);
		}
		PlayEffects(eyesend);
		PreviousFireTime = GetWorld()->TimeSeconds;
	}
}
	

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASWeapon::StartFire()
{
	
	float FireDelay =  FMath::Max(SecondsBetweenShots - (GetWorld()->TimeSeconds - PreviousFireTime), 0.0f);
	
	GetWorldTimerManager().SetTimer(TimerHandler,this, &ASWeapon::Fire, SecondsBetweenShots, true, FireDelay);;
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandler);
}
