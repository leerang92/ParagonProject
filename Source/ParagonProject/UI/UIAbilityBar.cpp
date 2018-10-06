// Fill out your copyright notice in the Description page of Project Settings.

#include "UIAbilityBar.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/SlateCore/Public/Styling/SlateBrush.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

void UUIAbilityBar::NativeConstruct()
{
	Super::NativeConstruct();
	
	NumOption.MaximumIntegralDigits = 1000;
	NumOption.MinimumFractionalDigits = 2;
	NumOption.MaximumFractionalDigits = 2;

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

	Texts.Emplace(CoolDownText_0);
	Texts.Emplace(CoolDownText_1);
	Texts.Emplace(CoolDownText_2);
	Texts.Emplace(CoolDownText_3);
	Texts.Emplace(CoolDownText_4);

	// ��� �ؽ��� ����
	for (auto& Img : BackgroundImgs)
	{
		Img->GetDynamicMaterial()->SetTextureParameterValue(TEXT("Texture"), BackgroundTexture);
	}
}

void UUIAbilityBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// ������Ʈ �迭�� ������� �ʴٸ� ������ ����
	if (UpdateAbilitys.Num() > 0)
	{
		for (int i = 0; i < UpdateAbilitys.Num(); ++i)
		{
			// Gauge ����
			UpdateAbilitys[i].Gauge -= InDeltaTime / UpdateAbilitys[i].CoolDown;
			// �̹��� �� �ؽ�Ʈ ����
			UpdateAbilitys[i].Image->GetDynamicMaterial()->SetScalarParameterValue(TEXT("Gauge"), UpdateAbilitys[i].Gauge);
			//UpdateAbilitys[i].Text->SetText(FText::AsNumber(UpdateAbilitys[i].Gauge * 100.0f, &NumOption));

			// Gauge�� ��� �����Ͽ� ��ų ��� �����ϵ��� �ʱ�ȭ
			if (UpdateAbilitys[i].Gauge <= 0.0f)
			{
				//SetBrush(UpdateAbilitys[i].AbilIcon, true);
				UpdateAbilitys[i].Image->GetDynamicMaterial()->SetScalarParameterValue(TEXT("Gauge"), 1.0f);
				UpdateAbilitys[i].Text->SetText(FText::GetEmpty());
				UpdateAbilitys.RemoveAt(i);
			}
		}
	}
}

void UUIAbilityBar::SetAbilityImage(UTexture * Image, const int Index)
{
	if (Image == nullptr)
	{
		UE_LOG(LogClass, Error, TEXT("Null Reference ability icon image"));
	}

	//Ability Image ����
	UImage* SetImage = Images[Index];
	SetImage->GetDynamicMaterial()->SetTextureParameterValue(TEXT("Texture"), Image);
}

void UUIAbilityBar::SetAbilityUI(const FAbilityInfo NewAbility)
{
	// ���� ��� ���� Ability ���� ����
	FUsableAbilityInfo AbilityInfo;
	const int Index = static_cast<uint8>(NewAbility.Type);

	// �̹���, ��Ÿ�� ����
	AbilityInfo.Image = BackgroundImgs[Index];
	AbilityInfo.AbilIcon = Images[Index];
	AbilityInfo.Text = Texts[Index];
	AbilityInfo.CoolDown = NewAbility.CoolDown;
	//SetBrush(AbilityInfo.AbilIcon, false);

	// Add update array
	UpdateAbilitys.Emplace(AbilityInfo);
}

void UUIAbilityBar::SetBrush(UImage * SetImage, bool bUsable)
{
	const float Alpha = (bUsable == true) ? 0 : -1.0f;
	SetImage->GetDynamicMaterial()->SetScalarParameterValue(TEXT("Alpha"), Alpha);
}
