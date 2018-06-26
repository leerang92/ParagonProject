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

	// ������Ʈ �迭
	TArray<FObjectInfo> Objects;

public:	
	// ������Ʈ ����
	void CreateObject(UWorld* World, UClass* Class, const FVector& Location, const FRotator& Rotator, const int SpawnNum);

	// ������Ʈ ��Ȱ��ȭ
	void SetDisable(const class AActor* Actor);

	// ������ƮǮ �ʱ�ȭ
	void Reset();

public:
	// ������Ʈ Ȱ��ȭ
	template<class T>
	T* ActiveObject();

};

template<class T>
inline T* UObjectPoolComponent::ActiveObject()
{
	// ������Ʈ�� ��Ȱ��ȭ�̸� Ȱ��ȭ �� ĳ�����Ͽ� ��ȯ
	for (auto& obj : Objects)
	{
		if (obj.bHidden)
		{	
			obj.bHidden = false;
			obj.Actor->SetActorHiddenInGame(false);
			return Cast<T>(obj.Actor);
		}
	}
	// ��ȯ�� ��ü�� ���� ��, �߰� ���� �� ��ȯ
	CreateObject(GetWorld(), Objects[0].Actor->GetClass(), FVector::ZeroVector, FRotator::ZeroRotator, 10);
	return ActiveObject<T>();
}
