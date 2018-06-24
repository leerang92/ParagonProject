// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/CharacterTypes.h"
#include "Runtime/SlateCore/Public/Styling/SlateBrush.h"
#include "UIAbilityBar.generated.h"

USTRUCT()
struct FUsableAbilityInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	class UImage* Image;

	float CoolDown;

	float Gauge;

	int UsageCountMax;

	int UsageCount;

	FUsableAbilityInfo() :
		Image(nullptr),
		CoolDown(0.0f),
		Gauge(1.0f),
		UsageCountMax(1),
		UsageCount(1)
	{}
};

/**
 * 
 */
UCLASS()
class PARAGONPROJECT_API UUIAbilityBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetImage(UTexture* Image, const int Index);

	void SetAbility(const FAbilityInfo Type);

	UPROPERTY(EditAnywhere)
	UTexture* BackgroundTexture;

private:
	// 사용한 스킬 배열
	UPROPERTY()
	TArray<FUsableAbilityInfo> UpdateAbilitys;

	/* 어빌리티 이미지 */
	UPROPERTY()
	TArray<class UImage*> Images;
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

	/* 배경 이미지 */
	UPROPERTY()
	TArray<class UImage*> BackgroundImgs;
	UPROPERTY()
	class UImage* Background_0;
	UPROPERTY()
	class UImage* Background_1;
	UPROPERTY()
	class UImage* Background_2;
	UPROPERTY()
	class UImage* Background_3;
	UPROPERTY()
	class UImage* Background_4;

	UPROPERTY()
	class UTextBlock* CoolDownText_0;
	UPROPERTY()
	class UTextBlock* CoolDownText_1;
	UPROPERTY()
	class UTextBlock* CoolDownText_2;
	UPROPERTY()
	class UTextBlock* CoolDownText_3;
	UPROPERTY()
	class UTextBlock* CoolDownText_4;
};
