// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/UIAbilityBar.h"
#include "Character/AbilityComponent.h"
#include "MainUMG.generated.h"

UCLASS()
class PARAGONPROJECT_API UMainUMG : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetupAbilityBar(UAbilityComponent& AbilityComp);

protected:
	UPROPERTY()
	class UUIAbilityBar* AbilityBar;
	
public:
	FORCEINLINE class UUIAbilityBar * GetAbilityBar() const { return AbilityBar; }
};
