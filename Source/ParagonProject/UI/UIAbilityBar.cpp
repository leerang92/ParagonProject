// Fill out your copyright notice in the Description page of Project Settings.

#include "UIAbilityBar.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

void UUIAbilityBar::NativeConstruct()
{
	Super::NativeConstruct();

	// �̹��� Ŭ���� �迭
	Images.Emplace(PrimaryImg);
	Images.Emplace(MouseRImg);
	Images.Emplace(Ability1Img);
	Images.Emplace(Ability2Img);
	Images.Emplace(UltimateImg);

	// ��� �̹��� �迭
	BackgroundImgs.Emplace(Background_0);
	BackgroundImgs.Emplace(Background_1);
	BackgroundImgs.Emplace(Background_2);
	BackgroundImgs.Emplace(Background_3);
	BackgroundImgs.Emplace(Background_4);

	// ��� �ؽ��� ����
	for (auto& Img : BackgroundImgs)
	{
		Img->GetDynamicMaterial()->SetTextureParameterValue(TEXT("Texture"), BackgroundTexture);
	}
}

void UUIAbilityBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// ������Ʈ �迭�� 0 �̻��̸�
	if (UpdateAbilitys.Num() > 0)
	{
		for (int i = 0; i < UpdateAbilitys.Num(); ++i)
		{
			// Gauge ����
			UpdateAbilitys[i].Gauge -= InDeltaTime / UpdateAbilitys[i].CoolDown;
			UpdateAbilitys[i].Image->GetDynamicMaterial()->SetScalarParameterValue(TEXT("Gauge"), UpdateAbilitys[i].Gauge);

			// Gauge�� ��� �����Ͽ� ��ų ��� �����ϵ��� �ʱ�ȭ
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
	//Ability Image ����
	UImage* SetImage = Images[Index];
	SetImage->GetDynamicMaterial()->SetTextureParameterValue(TEXT("Texture"), Image);
}

void UUIAbilityBar::SetAbility(const FAbilityInfo NewAbility)
{
	// ���� ��� ���� Ability ���� ����
	FUsableAbilityInfo AbilityInfo;
	const int Index = static_cast<uint8>(NewAbility.Type);
	// �̹���, ��Ÿ�� ����
	AbilityInfo.Image = BackgroundImgs[Index];
	AbilityInfo.CoolDown = NewAbility.CoolDown;

	// ������Ʈ �迭�� �߰�
	UpdateAbilitys.Emplace(AbilityInfo);
}
