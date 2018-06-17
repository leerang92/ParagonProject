// Fill out your copyright notice in the Description page of Project Settings.

#include "MainHUD.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

AMainHUD::AMainHUD()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTex(TEXT("Texture2D'/Game/Image/Crosshair/Crosshair_Image_2.Crosshair_Image_2'"));

	Crosshair = CrosshairTex.Object;
}

void AMainHUD::DrawHUD()
{
	FVector2D Center = FVector2D(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	FVector2D CrossHairDrawPosition(Center.X - (Crosshair->GetSurfaceWidth() * 0.5f), Center.Y - (Crosshair->GetSurfaceHeight() * 0.5f));

	
	FCanvasTileItem TileItem(CrossHairDrawPosition, Crosshair->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}
