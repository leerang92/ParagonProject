// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/CharacterTypes.h"
#include "AbilityComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARAGONPROJECT_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityComponent();

public:	
	TArray<FAbilityInfo> GetAbilityInfoAll();

	FAbilityInfo GetAbilityInfo(int Index);

	FAbilityInfo GetAbilityInfo(EAbilityType Type);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (AllowedPrivateAccess = true))
	TArray<FAbilityInfo> AbilityInfo;

};
