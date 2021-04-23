// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\SWeapon.h"
#include "PhysicsInterfacePhysX.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "SCharacter.h"
#include "CoopGame.h"
#include "UObject/ConstructorHelpers.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVarDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw debug lines"), ECVF_Cheat);
// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	socketname = "Muzzle";
	BulletsPerMin = 300.0f;
	PreviousFireTime = 0.0f;
	bIsFiring = false;
	bReplicates = true;
	SetReplicates(true);
	BulletSpreadDegrees = 1.0f;
	WeaponDamage = 30.0f;
	BackupBulletsRemaining = 124;
	MaxMagazineCapacity = 31;
	MagazineBulletsRemaining = MaxMagazineCapacity;

	//static ConstructorHelpers::FObjectFinder<USoundCue> propellerCue(
	//	TEXT("'/Game/airplane-engine.airplane-engine'")
	//);
	//// Store a reference to the Cue asset - we'll need it later.
	//propellerAudioCue = propellerCue.Object;

	//propellerAudioComponent = CreateDefaultSubobject<UAudioComponent>(
	//	TEXT("PropellerAudioComp")
	//	);
	//propellerAudioComponent->bAutoActivate = false;
	//propellerAudioComponent->AttachParent = RootComponent;
	//// I want the sound to come from slighty in front of the pawn.
	//propellerAudioComponent->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
	//// Attach our sound cue to the SoundComponent (outside the constructor)
	//if (propellerAudioCue->IsValidLowLevelFast()) {
	//	propellerAudioComponent->SetSound(propellerAudioCue);
	//}
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	bIsFiring = false;
	if(BulletsPerMin)
	{
		SecondsBetweenShots = 60 / BulletsPerMin;
	}
}

void ASWeapon::PlayEffects(FVector eyesend)
{
	if (smokeparticle)
	{
		FVector muzzlelocation = StaticMeshComp->GetSocketLocation(socketname);
		UParticleSystemComponent* impactcomp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), smokeparticle, muzzlelocation);
		if (impactcomp)
		{
			impactcomp->SetVectorParameter("Target", eyesend);
		}
	}

    if (muzzleflash)
    {
	    UGameplayStatics::SpawnEmitterAttached(muzzleflash, StaticMeshComp, socketname);
    }

	APawn* pawnowner = Cast<APawn>(GetOwner());
	if (pawnowner)
	{
		APlayerController* controller = Cast<APlayerController>(pawnowner->GetController());
		if(controller && CameraShakeWeapon)
		controller->ClientStartCameraShake(CameraShakeWeapon);
	}
}

void ASWeapon::Fire()
{
	// Don't fire if no more bullets in charger
	if (MagazineBulletsRemaining == 0)
		return;
	if (GetLocalRole() != ENetRole::ROLE_Authority)
	{
		ServerFire();
	}
	
	bIsFiring = true;
	AActor* owningactor = GetOwner();
	ASCharacter* FiringCharacter = Cast<ASCharacter>(owningactor);
	// Prevent from firing if owner is dead/destroyed
	if (owningactor && !FiringCharacter->bIsDead)
	{
		FiringCharacter->bIsFiring = true;
		FVector eyesvector;
		FRotator eyesrotation;
		owningactor->GetActorEyesViewPoint(eyesvector, eyesrotation);
		FVector shotdirection = eyesrotation.Vector();
		FVector eyesend = eyesvector + shotdirection * 10000;

		float angle_rad = FMath::DegreesToRadians(BulletSpreadDegrees);
		FMath::VRandCone(eyesend, angle_rad);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(owningactor);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;
		FHitResult hitresult;
		if (GetWorld()->LineTraceSingleByChannel(hitresult, eyesvector, eyesend, ECC_Weapon, QueryParams))
		{
			AActor* hitactor = hitresult.GetActor();
			ASCharacter* HitCharacter = Cast<ASCharacter>(hitactor);
			if (HitCharacter)
			{
				HitCharacter->BulletReceived(hitresult.BoneName);
			}
			eyesend = hitresult.ImpactPoint;
			UGameplayStatics::ApplyPointDamage(hitactor, WeaponDamage, shotdirection, hitresult, owningactor->GetInstigatorController(), owningactor, damagetype);
			

			EPhysicalSurface HitSurface = UPhysicalMaterial::DetermineSurfaceType(hitresult.PhysMaterial.Get());
			PlayFireImpact(HitSurface, eyesend);
			if (GetLocalRole() == ENetRole::ROLE_Authority)
			{
				HitScanStruct.LineTraceEndElement = eyesend;
				HitScanStruct.PhysicalSurfaceElement = HitSurface;	
			}
			Cast<ASCharacter>(GetOwner())->bIsFiring = true;
		}
		PlayEffects(eyesend);
		
		PreviousFireTime = GetWorld()->TimeSeconds;
		Cast<ASCharacter>(GetOwner())->bIsFiring = false;
		MagazineBulletsRemaining -= 1;
	}
	
}
	

void ASWeapon::Reload()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Weapon RELOAD: "));
	bIsReloading = true;
	// If there is enough bullets in backup to compensate magazine drain 
	if (BackupBulletsRemaining - (MaxMagazineCapacity - MagazineBulletsRemaining) > 0)
	{
		BackupBulletsRemaining -= (MaxMagazineCapacity - MagazineBulletsRemaining);
		MagazineBulletsRemaining = MaxMagazineCapacity;
	}
	else
	{
		MagazineBulletsRemaining += BackupBulletsRemaining;
		BackupBulletsRemaining = 0;
	}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ASWeapon::CanReload()
{
	bool result = false;
	if (MagazineBulletsRemaining < MaxMagazineCapacity && BackupBulletsRemaining > 0)
		result = true;
	return result;
}


void ASWeapon::StartFire()
{
	
	float FireDelay =  FMath::Max(SecondsBetweenShots - (GetWorld()->TimeSeconds - PreviousFireTime), 0.0f);
	
	GetWorldTimerManager().SetTimer(TimerHandler,this, &ASWeapon::Fire, SecondsBetweenShots, true, FireDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandler);
	AActor* owningactor = GetOwner();
	ASCharacter* FiringCharacter = Cast<ASCharacter>(owningactor);
	FiringCharacter->bIsFiring = false;
}

void ASWeapon::RefillAmmo()
{
	BackupBulletsRemaining = MaxBackupBullets;
}

void ASWeapon::ServerFire_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Server fire!!"));
	Fire();
}

bool ASWeapon::ServerFire_Validate()
{
	return true;
}

void ASWeapon::Client_FireEffects()
{
	PlayFireImpact(HitScanStruct.PhysicalSurfaceElement, HitScanStruct.LineTraceEndElement);
	PlayEffects(HitScanStruct.LineTraceEndElement);
	
}

void ASWeapon::PlayFireImpact(EPhysicalSurface HitSurface, FVector TraceEnd)
{

	UParticleSystem* ImpactEffect;
	switch (HitSurface)
	{
	case SURFACE_TYPE_DEFAULT:
		ImpactEffect = DefaultImpactEffect;
		break;
	case SURFACE_TYPE_FLESH:
		ImpactEffect = FleshImpactEffect;
		break;
	case SURFACE_TYPE_ROCK:
		ImpactEffect = RockImpactEffect;
		break;
	case SURFACE_TYPE_WATER:
		ImpactEffect = WaterImpactEffect;
		break;
	case SURFACE_TYPE_EXPLODE:
		ImpactEffect = DefaultImpactEffect;
		break;
	default:
		ImpactEffect = DefaultImpactEffect;
		break;
	}

	FVector eyesvector;
	FRotator eyesrotation;
	GetOwner()->GetActorEyesViewPoint(eyesvector, eyesrotation);
	FVector shotdirection = eyesrotation.Vector();
	FVector eyesend = eyesvector + shotdirection * 10000;
	shotdirection.Normalize();

	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, TraceEnd, shotdirection.Rotation());
	}
}

void ASWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ASWeapon, HitScanStruct, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASWeapon, bIsFiring, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASWeapon, bIsReloading, COND_SkipOwner);
}
