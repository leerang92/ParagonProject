// Fill out your copyright notice in the Description page of Project Settings.

#include "UIAbilityBar.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

void UUIAbilityBar::NativeConstruct()
{
	Super::NativeConstruct();

	// 이미지 클래스 배열
	Images.Emplace(PrimaryImg);
	Images.Emplace(MouseRImg);
	Images.Emplace(Ability1Img);
	Images.Emplace(Ability2Img);
	Images.Emplace(UltimateImg);

	// 배경 이미지 배열
	BackgroundImgs.Emplace(Background_0);
	BackgroundImgs.Emplace(Background_1);
	BackgroundImgs.Emplace(Background_2);
	BackgroundImgs.Emplace(Background_3);
	BackgroundImgs.Emplace(Background_4);

	// 배경 텍스쳐 설정
	for (auto& Img : BackgroundImgs)
	{
		Img->GetDynamicMaterial()->SetTextureParameterValue(TEXT("Texture"), BackgroundTexture);
	}
}

void UUIAbilityBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 업데이트 배열이 0 이상이면
	if (UpdateAbilitys.Num() > 0)
	{
		for (int i = 0; i < UpdateAbilitys.Num(); ++i)
		{
			// Gauge 감소
			UpdateAbilitys[i].Gauge -= InDeltaTime / UpdateAbilitys[i].CoolDown;
			UpdateAbilitys[i].Image->GetDynamicMaterial()->SetScalarParameterValue(TEXT("Gauge"), UpdateAbilitys[i].Gauge);

			// Gauge가 모두 감소하여 스킬 사용 가능하도록 초기화
			if (UpdateAbilitys[i].Gauge <= 0.0f)
			{
				UpdateAbilitys[i].Image->GetDynamicMaterial()->SetScalarParameterValue(TEXT("Gauge"), 1.0f);
				UpdateAbilitys.RemoveAt(i);
			}
		}
	}
}

void UUIAbilityBar::SetImage(UTexture * Image, const int Index)
{
	//Ability Image 설정
	UImage* SetImage = Images[Index];
	SetImage->GetDynamicMaterial()->SetTextureParameterValue(TEXT("Texture"), Image);
}

void UUIAbilityBar::SetAbility(const FAbilityInfo NewAbility)
{
	// 현재 사용 중인 Ability 정보 설정
	FUsableAbilityInfo AbilityInfo;
	const int Index = static_cast<uint8>(NewAbility.Type);
	// 이미지, 쿨타임 설정
	AbilityInfo.Image = BackgroundImgs[Index];
	AbilityInfo.CoolDown = NewAbility.CoolDown;

	// 업데이트 배열에 추가
	UpdateAbilitys.Emplace(AbilityInfo);
}
