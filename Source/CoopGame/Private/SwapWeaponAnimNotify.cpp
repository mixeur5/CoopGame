// Fill out your copyright notice in the Description page of Project Settings.


#include "SwapWeaponAnimNotify.h"
#include "SCharacter.h"

void USwapWeaponAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ASCharacter* OwningSCharacter = Cast<ASCharacter>(MeshComp->GetOwner());
	OwningSCharacter->SwitchSockets(OwningSCharacter->CurrentWeaponIndex);
}
