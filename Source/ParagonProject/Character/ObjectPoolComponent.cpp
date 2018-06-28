// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectPoolComponent.h"



// Sets default values for this component's properties
UObjectPoolComponent::UObjectPoolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	Index = 0;
	MaxObjNum = 0;
}


// Called when the game starts
void UObjectPoolComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	
}

void UObjectPoolComponent::CreateObject(UWorld * World, UClass * Class, const FVector & Location, const FRotator & Rotator, const int SpawnNum)
{
	MaxObjNum = SpawnNum;
	for (int i = 0; i < MaxObjNum; ++i)
	{
		FObjectInfo Info;
		Info.Actor = World->SpawnActor<AActor>(Class, Location, Rotator);
		Info.Actor->SetFolderPath("Character ObjectPool");
		Info.Actor->SetActorHiddenInGame(true);

		Objects.Add(Info);
	}
}

void UObjectPoolComponent::SetDisable(const AActor * Actor)
{
	for (auto& obj : Objects)
	{
		if (obj.Actor == Actor)
		{
			obj.Actor->SetActorHiddenInGame(true);
			obj.bHidden = true;
		}
	}
}

void UObjectPoolComponent::Reset()
{
	for (auto& obj : Objects)
	{
		obj.Actor->SetLifeSpan(0.01f);
		obj.Actor = nullptr;
	}
	Objects.Reset();
}



