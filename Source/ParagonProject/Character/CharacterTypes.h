// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Engine/Texture.h"
#include "CharacterTypes.generated.h"

UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	Primary,
	MouseR,
	Ability1,
	Ability2,
	Ultimate,
};

USTRUCT(BlueprintType)
struct FAbilityInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Ability")
	UTexture* Image;

	UPROPERTY(EditAnywhere, Category = "Ability")
	FName Name;

	UPROPERTY(EditAnywhere, Category = "Ability")
	float CoolDown;

	UPROPERTY(EditAnywhere, Category = "Ability")
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Ability")
	EAbilityType Type;

	UPROPERTY(EditAnywhere, Category = "Ability")
	int UsageCountMax;

	FAbilityInfo() :
		Image(nullptr),
		Name(TEXT("Ability Name")),
		UsageCountMax(1)
	{}
};