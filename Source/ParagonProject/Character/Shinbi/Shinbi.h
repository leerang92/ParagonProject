// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/ObjectPoolComponent.h"
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
	UPROPERTY(EditAnywhere, Category = "ObjectPool")
	class UObjectPoolComponent* ObjPoolComp;

	// 생성할 울프 객체 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Wolf")
	TSubclassOf<class AShinbiWolf> WolfClass;

	// 생성할 늑대 수
	UPROPERTY(EditDefaultsOnly, Category = "Wolves")
	int MaxWolfNum;

	// Dash 스킬 나아갈 크기 변수
	UPROPERTY(EditAnywhere, Category = "Ability")
	float DashPower;

	UPROPERTY(EditAnywhere, Category = "Ability")
	float AttackWolvesDuration;

	// CirclingWolves(Abiility2) 지속 시간 변수
	UPROPERTY(EditAnywhere, Category = "Ability")
	float CirclingWolvesDuration;

	// Primary 스킬 각 늑대들의 각도 간격
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float UltimateAngleInterval;

	// Primary 스킬 늑대 생성 시간
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float UltimateSpawnTime;

	void UltimateHitNotify();

public:
	// 대쉬 스킬 사용시 생성할 파티클
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* DashParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* UltMarker;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* UltMarkerDestroy;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* UltFinalFX;

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

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* UltMontage;

	UPROPERTY(EditAnywhere, Category = Camera)
	TSubclassOf<UCameraShake> CamShake;

protected:
	/* Attack */
	virtual void StartAttack() override;

	virtual void ComboAttack() override;

	FTimerHandle AttackEndTimer;

	/* Ability */
	virtual void Ultimate() override;
	virtual void Ability1() override;
	virtual void Ability2() override;
	virtual void AbilityMouseR() override;

private:
	FTimerHandle AttackWolvesTimer;

	// Attack Wolves 스킬 캐스트 중단 및 늑대 소환
	void StopAttackCast();

	void StopAttackWolves(AShinbiWolf* Wolf);

	// Circling Wolves 스킬 사용시 생성된 늑대 인덱스들
	TSet<AShinbiWolf*> CirclingWolfSet;

	// Circle Wolves 스킬 중단 타이머핸들
	FTimerHandle CircleWolvesTimer;

	// Circle Wolves 스킬 사용시 늑대 생성 간격
	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (AllowedPrivateAccess = "true"))
	float CircleWolfRate;

	bool bIsCircling : 1;

	// Circle Wolves 스킬 중단
	void StopCircleWolves();

	FVector SetAngle(FVector NewLocation, float NewAngle);

	bool bIsUltimate : 1;

	int UltHitCount;

	// Primary 스킬 늑대 생성 타이머핸들
	FTimerHandle UltSpawnTimers[8];

	class APawn* TargetPawn;

	FTimerHandle UltShakeTimer;

	void CameraShake();

	FTimerHandle MarkerTimer;

	void DestroyMarker();

	UParticleSystemComponent* MarkerFX;

private:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
