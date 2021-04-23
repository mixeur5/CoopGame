// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "SPowerUpActor.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	SphereComp->SetSphereRadius(75);
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64, 75, 75);
	RootComponent = SphereComp;
	DecalComp->SetupAttachment(RootComponent);
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		Respawn();
	}
		
}

void ASPickupActor::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if ((GetLocalRole() == ENetRole::ROLE_Authority) && PowerUpInstance)
	{
		PowerUpInstance->ActivatePowerup(OtherActor);
		PowerUpInstance = nullptr;
		GetWorldTimerManager().SetTimer(TimerHandle_Cooldown, this, &ASPickupActor::Respawn, CooldownDuration);
	}
}

void ASPickupActor::Respawn()
{
	if (PowerUpClass == nullptr)
	{
		return;
	}
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PowerUpInstance = GetWorld()->SpawnActor<ASPowerUpActor>(PowerUpClass, GetActorLocation(), GetActorRotation(), ActorSpawnParams);
}

