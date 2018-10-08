// Fill out your copyright notice in the Description page of Project Settings.

#include "SerathAnimInst.h"

void USerathAnimInst::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerPawn = Cast<ASerath>(TryGetPawnOwner());

	if (OwnerPawn == nullptr) 
	{
		UE_LOG(LogClass, Error, TEXT("Can't find owner pawn class"));
	}
}

void USerathAnimInst::AnimNotify_NextAttack()
{
	OwnerPawn->ComboAttack();
}

void USerathAnimInst::AnimNotify_StopAttack()
{
	OwnerPawn->ResetComboAttack();
}

void USerathAnimInst::AnimNotify_FuryFly()
{
	OwnerPawn->SetFly();
}

void USerathAnimInst::AnimNotify_FuryDamage()
{
	OwnerPawn->OnFuryDamage();
}

