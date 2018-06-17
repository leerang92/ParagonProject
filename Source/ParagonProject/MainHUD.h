// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Runtime/Engine/Classes/Engine/Canvas.h"
#include "MainHUD.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONPROJECT_API AMainHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AMainHUD();

	void DrawHUD();
	
	UPROPERTY()
	UTexture2D* Crosshair;
};
