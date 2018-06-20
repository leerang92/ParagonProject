// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/CharacterTypes.h"
#include "Runtime/SlateCore/Public/Styling/SlateBrush.h"
#include "UIAbilityBar.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONPROJECT_API UUIAbilityBar : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void Init();

	void SetPrimaryImage(UTexture2D* Image, const EAbilityType Type);

protected:
	UPROPERTY()
	class UImage * PrimaryImg;

	UPROPERTY()
	class UImage * MouseRImg;

	UPROPERTY()
	class UImage * Ability1Img;

	UPROPERTY()
	class UImage * Ability2Img;

	UPROPERTY()
	class UImage * UltimateImg;

private:
	void SetImageStyle(UImage* SetImage, UTexture2D* NewTexture);

};
