// Fill out your copyright notice in the Description page of Project Settings.

#include "UIAbilityBar.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/Image.h"

void UUIAbilityBar::SetPrimaryImage(UTexture2D * Image, const EAbilityType Type)
{
	UImage* SetImage = nullptr;

	switch (Type)
	{
	case EAbilityType::Primary:
		SetImage = PrimaryImg;
		break;
	case EAbilityType::MouseR:
		SetImage = MouseRImg;
		break;
	case EAbilityType::Ability1:
		SetImage = Ability1Img;
		break;
	case EAbilityType::Ability2:
		SetImage = Ability2Img;
		break;
	case EAbilityType::Ultimate:
		SetImage = UltimateImg;
		break;
	}

	SetImageStyle(SetImage, Image);
}

void UUIAbilityBar::SetImageStyle(UImage * SetImage, UTexture2D * NewTexture)
{
	FSlateBrush NewBrush;

	NewBrush.ImageSize = FVector2D(85.0f, 85.0f);
	NewBrush.SetResourceObject(NewTexture);
	

	SetImage->SetBrush(NewBrush);
}
