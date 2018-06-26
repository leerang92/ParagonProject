// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "ObjectPoolComponent.generated.h"


USTRUCT()
struct FObjectInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	class AActor* Actor;

	bool bHidden;

	FObjectInfo() :
		Actor(nullptr),
		bHidden(true)
	{}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARAGONPROJECT_API UObjectPoolComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UObjectPoolComponent();

	virtual void BeginPlay() override;

	// 오브젝트 배열
	TArray<FObjectInfo> Objects;

public:	
	// 오브젝트 생성
	void CreateObject(UWorld* World, UClass* Class, const FVector& Location, const FRotator& Rotator, const int SpawnNum);

	// 오브젝트 비활성화
	void SetDisable(const class AActor* Actor);

	// 오브젝트풀 초기화
	void Reset();

public:
	// 오브젝트 활성화
	template<class T>
	T* ActiveObject();

};

template<class T>
inline T* UObjectPoolComponent::ActiveObject()
{
	// 오브젝트가 비활성화이면 활성화 후 캐스팅하여 반환
	for (auto& obj : Objects)
	{
		if (obj.bHidden)
		{	
			obj.bHidden = false;
			obj.Actor->SetActorHiddenInGame(false);
			return Cast<T>(obj.Actor);
		}
	}
	// 반환할 객체가 없을 시, 추가 생성 후 반환
	CreateObject(GetWorld(), Objects[0].Actor->GetClass(), FVector::ZeroVector, FRotator::ZeroRotator, 10);
	return ActiveObject<T>();
}
