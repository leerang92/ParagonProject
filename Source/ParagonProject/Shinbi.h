// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
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

	// Dash 스킬 나아갈 크기 변수
	UPROPERTY(EditAnywhere, Category = "Ability")
	float DashPower;

	// CirclingWolves(Abiility2) 지속 시간 변수
	UPROPERTY(EditAnywhere, Category = "Ability")
	float CirclingWolvesDuration;

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

	virtual void StartAttack() override;

	virtual void StopAttack() override;

	virtual void ComboAttack() override;

	virtual void ResetComboAttack() override;

	FTimerHandle AttackTimer;

	/* Ability */
	virtual void PrimaryAbility() override;
	virtual void Ability1() override;
	virtual void Ability2() override;
	virtual void AbilityMouseR() override;

	// Attack Wolves 스킬 캐스트 중단 및 늑대 소환
	void StopAttackCast();

	// Circle Wolves 스킬 중단
	void StopCircleWolves();

	// Circle Wolves 스킬 중단 타이머핸들
	FTimerHandle CircleWolvesTimer;

private:
	UPROPERTY(Transient)
	TArray<class AShinbiWolf*> Wolves;

	// Circle Wolves 스킬 사용시 늑대 생성 간격
	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (AllowedPrivateAccess = "true"))
	float WolfInterval;

	int WolfIndex;

	void CreateWolves();

	void EnableWolves(FVector InitVec = FVector::ZeroVector, FRotator InitRot = FRotator::ZeroRotator);

	void DisableWolves();
};
