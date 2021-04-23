// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

ASProjectile::ASProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(0.5f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASProjectile::OnHit);	// set up a notification for when this component hits something blocking

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Important! Ignore Visibility because this channel is used by explosion damage !
	// This will block damage otherwise!
	CollisionComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	// Set as root component
	RootComponent = CollisionComp;
	MeshComp->SetupAttachment(CollisionComp);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3500.f;
	ProjectileMovement->MaxSpeed = 3500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	ExplodingParticleSystem = CreateDefaultSubobject<UParticleSystem>(TEXT("ExplodingSystem"));
	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForce->Radius = 100.0f;
	RadialForce->ImpulseStrength = 50.0f;
	RadialForce->AddCollisionChannelToAffect(ECollisionChannel::ECC_PhysicsBody);
	RadialForce->SetupAttachment(RootComponent);
	RadialForce->SetAutoActivate(false);

	// Die after 3 seconds by default
	//InitialLifeSpan = 3.0f;
	SetReplicates(true);
	SetReplicateMovement(true);
}

void ASProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}
	MakeNoise(1.0f);
	ProjectileExplosion();
}

void ASProjectile::ProjectileExplosion()
{
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplodingParticleSystem, GetActorLocation(), FRotator::ZeroRotator, 10.0f);
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		RadialForce->FireImpulse();
		//UGameplayStatics::ApplyRadialDamage(this, 5000.0f, GetActorLocation(), 4000.0f, UDamageType::StaticClass(), TArray<AActor*>());
		UGameplayStatics::ApplyRadialDamageWithFalloff(this, 300.0f, 100.0f, GetActorLocation(), 100.0f, 300.0f, 3.0f, UDamageType::StaticClass(), TArray<AActor*>());
		Destroy();
	}
}
