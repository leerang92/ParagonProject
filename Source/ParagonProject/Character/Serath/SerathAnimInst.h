// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/Serath/Serath.h"
#include "SerathAnimInst.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONPROJECT_API USerathAnimInst : public UAnimInstance
{
	GENERATED_BODY()
	
private:

	virtual void NativeInitializeAnimation() override;

	UFUNCTION()
	void AnimNotify_NextAttack();

	UFUNCTION()
	void AnimNotify_StopAttack();

	UFUNCTION()
	void AnimNotify_FuryFly();

	UFUNCTION()
	void AnimNotify_FuryDamage();
	
	UPROPERTY()
	class ASerath* OwnerPawn;
};
