// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityComponent.h"
#include "UI/UIAbilityBar.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"


// Sets default values for this component's properties
UAbilityComponent::UAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UAbilityComponent::UseAbility(const EAbilityType Type)
{
	switch (Type)
	{
	case EAbilityType::Primary:
		break;
	case EAbilityType::MouseR:
		break;
	case EAbilityType::Ability1:
		break;
	case EAbilityType::Ability2:
		break;
	case EAbilityType::Ultimate:
		break;
	}
}

FAbilityInfo UAbilityComponent::GetAbilityInfo(int Index) const
{
	return AbilityInfo[Index];
}

void UAbilityComponent::SetPrimary()
{
}

void UAbilityComponent::SetMouseR()
{
}

void UAbilityComponent::SetAbility1()
{
}

void UAbilityComponent::SetAbility2()
{
}

void UAbilityComponent::SetUltimate()
{
}


