// Fill out your copyright notice in the Description page of Project Settings.

#include "Serath.h"

ASerath::ASerath()
{
	CurrentAbility = EAbilityType::None;

	SectionCount = 0;

	// Primary 몽타주 애니메이션 섹션들
	PrimarySectionNames.Add(TEXT("A_Start"));
	PrimarySectionNames.Add(TEXT("B_Start"));
	PrimarySectionNames.Add(TEXT("C_Start"));
	PrimarySectionNames.Add(TEXT("D_Start"));
}

void ASerath::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASerath::StartPrimary()
{
	Super::StartPrimary();
	CurrentAbility = EAbilityType::Primary;
}

void ASerath::ComboAttack()
{
	if (SaveCombo && AttackMontages.Num() > 0)
	{
		PlayAnimMontage(AttackMontages[0], 1.5f, PrimarySectionNames[SectionCount++]);

		if (PrimarySectionNames.Num() <= SectionCount)
		{
			SectionCount = 0;
		}
	}
}

void ASerath::ResetComboAttack()
{
	Super::ResetComboAttack();
	SectionCount = 0;
}

void ASerath::AbilityMR()
{
	Super::AbilityMR();

	PlayAnimMontage(WingFlapAnim);
	
}

void ASerath::AbilityQ()
{
	if (CurrentAbility != EAbilityType::AbilityQ)
	{
		bAbilityFury = true;
		CurrentAbility = EAbilityType::AbilityQ;
		PlayAnimMontage(FuryAnim);
	}
	else if (CurrentAbility == EAbilityType::AbilityQ && bAbilityFury)
	{
		bAbilityFury = true;
		CurrentAbility = EAbilityType::None;
		PlayAnimMontage(FuryAnim, 1.0f, TEXT("Cancel"));
	}
}

void ASerath::AbilityE()
{
	Super::AbilityE();

}

