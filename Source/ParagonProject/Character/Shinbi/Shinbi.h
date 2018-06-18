// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Shinbi.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONPROJECT_API AShinbi : public ABaseCharacter
{
	GENERATED_BODY()

private:
	AShinbi();

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	// 생성할 울프 객체 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Wolf")
	TSubclassOf<class AShinbiWolf> WolfClass;

	// 생성할 늑대 수
	UPROPERTY(EditDefaultsOnly, Category = "Wolves")
	int MaxWolfNum;

	// Dash 스킬 나아갈 크기 변수
	UPROPERTY(EditAnywhere, Category = "Ability")
	float DashPower;

	// CirclingWolves(Abiility2) 지속 시간 변수
	UPROPERTY(EditAnywhere, Category = "Ability")
	float CirclingWolvesDuration;

	// Primary 스킬 각 늑대들의 각도 간격
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float UltimateAngleInterval;

	// Primary 스킬 늑대 생성 시간
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float UltimateSpawnTime;

	// 대쉬 스킬 사용시 생성할 파티클
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* DashParticle;

	/* 스킬 몽타주 애니메이션 */
	// Dash 스킬 몽타주 애니메이션
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* DashMontage;

	// AttackWolves 스킬 몽타주 애니메이션
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* AttackWolvesMontage;

	// CircleWolves 스킬 몽타주 애니메이션
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* CircleWolvesMontage;

protected:
	virtual void Tick(float DeltaTime) override;

	/* Attack */
	virtual void StartAttack() override;

	virtual void ComboAttack() override;

	FTimerHandle AttackTimer;

	/* Ability */
	virtual void Ultimate() override;
	virtual void Ability1() override;
	virtual void Ability2() override;
	virtual void AbilityMouseR() override;

	// Attack Wolves 스킬 캐스트 중단 및 늑대 소환
	void StopAttackCast();

	// Circle Wolves 스킬 중단
	void StopCircleWolves();

	/* Primary 스킬 */
	void SetUltimateWolves(int Index);

	FVector SetAngle(FVector NewLocation, float NewAngle);
private:
	UPROPERTY(Transient)
	TArray<class AShinbiWolf*> Wolves;

	// Circling Wolves 스킬 사용시 생성된 늑대 인덱스들
	TSet<int> CirclingIndexes;

	// Circle Wolves 스킬 사용시 늑대 생성 간격
	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (AllowedPrivateAccess = "true"))
	float WolfInterval;

	bool bIsCircling : 1;

	bool bIsUltimate : 1;

	// Circle Wolves 스킬 중단 타이머핸들
	FTimerHandle CircleWolvesTimer;

	// Primary 스킬 늑대 생성 타이머핸들
	FTimerHandle SpawnWolvesTimer[8];

	// 현재 늑대 배열의 인덱스
	int WolfIndex;

	// 늑대 활성화 함수
	TSet<int> SetupWolves(const FVector SpawnVec, const FRotator SpawnRot, uint8 Type, int SpawnNum = 1);

	// 게임 시작시 늑대 생성
	void CreateWolves();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
