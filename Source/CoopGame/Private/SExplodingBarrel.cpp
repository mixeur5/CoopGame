// Fill out your copyright notice in the Description page of Project Settings.

#include "SExplodingBarrel.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "SHealthComponent.h"
#include "Particles/ParticleSystem.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASExplodingBarrel::ASExplodingBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	MaterialInterface = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material"));
	ExplodingParticleSystem = CreateDefaultSubobject<UParticleSystem>(TEXT("ExplodingSystem"));
	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MEshComp"));
	RootComponent = MeshComponent;
	RadialForce->Radius = 100.0f;
	RadialForce->ImpulseStrength = 50.0f;
	RadialForce->AddCollisionChannelToAffect(ECollisionChannel::ECC_PhysicsBody);
	RadialForce->SetupAttachment(RootComponent);
	RadialForce->SetAutoActivate(false);
	bIsDestroyed = false;
	MeshComponent->SetSimulatePhysics(true);
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASExplodingBarrel::BeginPlay()
{
	Super::BeginPlay();
	HealthComp->HealthChangedEvent.AddDynamic(this, &ASExplodingBarrel::ActorTakingDamage);
	RadialForce->AddCollisionChannelToAffect(ECollisionChannel::ECC_WorldDynamic);
	MeshComponent->SetSimulatePhysics(true);
}


void ASExplodingBarrel::ActorTakingDamage(USHealthComponent* HealthCompParam, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (HealthCompParam->CurrentHealth <= 0.0f && !bIsDestroyed)
	{
		BarrelExplosion();
		bIsDestroyed = true;
	}
}

void ASExplodingBarrel::BarrelExplosion()
{
	MeshComponent->AddImpulse(FVector::UpVector * 1000, NAME_None, true);
    UGameplayStatics::SpawnEmitterAtLocation(this, ExplodingParticleSystem, GetActorLocation(), FRotator::ZeroRotator, 10.0f);
    MeshComponent->SetMaterial(0, MaterialInterface);
    RadialForce->FireImpulse();
}

void ASExplodingBarrel::Client_BarrelExplosion()
{
	if (bIsDestroyed)
	{
		BarrelExplosion();
	}
}

void ASExplodingBarrel::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASExplodingBarrel, bIsDestroyed);
}


