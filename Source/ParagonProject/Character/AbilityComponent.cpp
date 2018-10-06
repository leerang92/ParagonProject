// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityComponent.h"
#include "UI/UIAbilityBar.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"


// Sets default values for this component's properties
UAbilityComponent::UAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

TArray<FAbilityInfo> UAbilityComponent::GetAbilityInfoAll()
{
	return AbilityInfo;
}

FAbilityInfo UAbilityComponent::GetAbilityInfo(int Index)
{
	return AbilityInfo[Index];
}

FAbilityInfo UAbilityComponent::GetAbilityInfo(EAbilityType Type)
{
	for (auto& info : AbilityInfo)
	{
		if (info.Type == Type)
		{
			return info;
		}
	}

	UE_LOG(LogClass, Error, TEXT("Find not ability type"));
	return FAbilityInfo();
}