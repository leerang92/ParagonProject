// Fill out your copyright notice in the Description page of Project Settings.

#include "ParagonProjectGameModeBase.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

AParagonProjectGameModeBase::AParagonProjectGameModeBase()
{
	static ConstructorHelpers::FObjectFinder<UClass> HUDFinder(TEXT("Class'/Script/ParagonProject.MainHUD'"));

	HUDClass = (UClass*)HUDFinder.Object;
}


