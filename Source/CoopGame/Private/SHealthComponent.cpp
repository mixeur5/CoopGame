// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "SHealthComponent.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	TeamNumber = 0;
	// ...
}

// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = DefaultHealth;
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		if (GetOwner())
		{
			AActor* owner = GetOwner();
			owner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::TakingDamage);
		}
	}
}

void USHealthComponent::TakingDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser )
{
	if(CurrentHealth <= Damage)
	{
        CurrentHealth = 0.0f;
	}
	    
	else
	{
		CurrentHealth = CurrentHealth - Damage;	/* code */
	}
	
	HealthChangedEvent.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);	
}

bool USHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		return false;
	}
	USHealthComponent* HealthCompA = Cast<USHealthComponent>(ActorA->GetComponentByClass(USHealthComponent::StaticClass()));
	USHealthComponent* HealthCompB = Cast<USHealthComponent>(ActorB->GetComponentByClass(USHealthComponent::StaticClass()));
	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		return false;
	}
	return HealthCompA->TeamNumber == HealthCompB->TeamNumber;
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USHealthComponent, CurrentHealth);
}
