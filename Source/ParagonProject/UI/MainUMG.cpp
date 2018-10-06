// Fill out your copyright notice in the Description page of Project Settings.

#include "MainUMG.h"

void UMainUMG::SetupAbilityBar(UAbilityComponent& AbilityComp)
{
	auto AbilityInfoArr = AbilityComp.GetAbilityInfoAll();
	for (int i = 0; i < AbilityInfoArr.Num(); ++i)
	{
		if (AbilityInfoArr[i].Image != nullptr)
		{
			AbilityBar->SetAbilityImage(AbilityInfoArr[i].Image, i);
		}
	}
}