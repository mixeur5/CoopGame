// Fill out your copyright notice in the Description page of Project Settings.


#include "STrackerBot.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Runtime/NavigationSystem/Public/NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "SCharacter.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh component"));
	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	
	MeshComp->SetCanEverAffectNavigation(false);
	RootComponent = MeshComp ;
	ExplodingParticleSystem = CreateDefaultSubobject<UParticleSystem>(TEXT("ExplodingSystem"));
	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForce->Radius = 100.0f;
	RadialForce->ImpulseStrength = 50.0f;
	RadialForce->AddCollisionChannelToAffect(ECollisionChannel::ECC_PhysicsBody);
	RadialForce->SetupAttachment(RootComponent);
	RadialForce->SetAutoActivate(false);
	bIsDestroyed = false;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	if (GetLocalRole() == ROLE_Authority)
	{
		bIsStarting = true;
		RadialForce->AddCollisionChannelToAffect(ECollisionChannel::ECC_WorldDynamic);
		HealthComponent->HealthChangedEvent.AddDynamic(this, &ASTrackerBot::ActorTakingDamage);
	}
}

void ASTrackerBot::ActorTakingDamage(USHealthComponent * HealthCompParam, float Health, float HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (HealthComponent->GetHealth() <= 0.0f && !bIsDestroyed)
	{
		bIsDestroyed = true;
		BotExplosion();
		Destroy();
	}
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority)
	{
		AActor* character = UGameplayStatics::GetPlayerCharacter(this, 0);
		UNavigationPath* NavigationPath;
		if (bIsStarting && character)
		{
			NavigationPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), character);
			if (NavigationPath != nullptr && NavigationPath->PathPoints.Num() > 1)
			{
				for (int i = 0; i < NavigationPath->PathPoints.Num(); i++)
				{
					DrawDebugSphere(GetWorld(), NavigationPath->PathPoints[i], 20.0f, 12, FColor::Yellow, false, 5.0f);
				}
				NextPathPoint = NavigationPath->PathPoints[1];
				DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), NextPathPoint, 300.0f, FColor::Cyan, false, 10.0f, 0, 1.0f);
			}
			else
			{
				NextPathPoint = character->GetActorLocation();
			}
			bIsStarting = false;
		}
		/*DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), NextPathPoint, 300.0f, FColor::Cyan, false, 0.0f, 0, 1.0f);*/
		//size returns the "valeur absolue" of the vector
		float DistanceToNextPoint = (GetActorLocation() - NextPathPoint).Size();
		if (DistanceToNextPoint > 10.0f)
		{
			MeshComp->AddForce(NextPathPoint - GetActorLocation(), NAME_None, true);
		}
		else
		{
			NavigationPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), character);
			for (int i = 0; i < NavigationPath->PathPoints.Num(); i++)
			{
				DrawDebugSphere(GetWorld(), NavigationPath->PathPoints[i], 20.0f, 12, FColor::Yellow, false, 5.0f);
			}
			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), character->GetActorLocation(), 300.0f, FColor::Cyan, false, 0.0f, 0, 1.0f);
			if (NavigationPath != nullptr && NavigationPath->PathPoints.Num() > 1)
			{
				NextPathPoint = NavigationPath->PathPoints[1];
			}
		}
	}
}

	void ASTrackerBot::BotExplosion()
	{
		MeshComp->AddImpulse(FVector::UpVector * 1000, NAME_None, true);
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplodingParticleSystem, GetActorLocation());
		RadialForce->FireImpulse();
	}

	void ASTrackerBot::Client_BotExplosion()
	{
		if (bIsDestroyed)
		{
			BotExplosion();
		}
	}
			



