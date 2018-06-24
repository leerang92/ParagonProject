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

	// ������ ���� ��ü Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Wolf")
	TSubclassOf<class AShinbiWolf> WolfClass;

	// ������ ���� ��
	UPROPERTY(EditDefaultsOnly, Category = "Wolves")
	int MaxWolfNum;

	// Dash ��ų ���ư� ũ�� ����
	UPROPERTY(EditAnywhere, Category = "Ability")
	float DashPower;

	UPROPERTY(EditAnywhere, Category = "Ability")
	float AttackWolvesDuration;

	// CirclingWolves(Abiility2) ���� �ð� ����
	UPROPERTY(EditAnywhere, Category = "Ability")
	float CirclingWolvesDuration;

	// Primary ��ų �� ������� ���� ����
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float UltimateAngleInterval;

	// Primary ��ų ���� ���� �ð�
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float UltimateSpawnTime;

	void UltimateHitNotify();

public:
	// �뽬 ��ų ���� ������ ��ƼŬ
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* DashParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* UltMarker;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* UltMarkerDestroy;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* UltFinalFX;

	/* ��ų ��Ÿ�� �ִϸ��̼� */
	// Dash ��ų ��Ÿ�� �ִϸ��̼�
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* DashMontage;

	// AttackWolves ��ų ��Ÿ�� �ִϸ��̼�
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* AttackWolvesMontage;

	// CircleWolves ��ų ��Ÿ�� �ִϸ��̼�
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

	// Attack Wolves ��ų ĳ��Ʈ �ߴ� �� ���� ��ȯ
	void StopAttackCast();

	void StopAttackWolves(AShinbiWolf* Wolf);

	// Circling Wolves ��ų ���� ������ ���� �ε�����
	TSet<AShinbiWolf*> CirclingWolfSet;

	// Circle Wolves ��ų �ߴ� Ÿ�̸��ڵ�
	FTimerHandle CircleWolvesTimer;

	// Circle Wolves ��ų ���� ���� ���� ����
	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (AllowedPrivateAccess = "true"))
	float CircleWolfRate;

	bool bIsCircling : 1;

	// Circle Wolves ��ų �ߴ�
	void StopCircleWolves();

	FVector SetAngle(FVector NewLocation, float NewAngle);

	bool bIsUltimate : 1;

	int UltHitCount;

	// Primary ��ų ���� ���� Ÿ�̸��ڵ�
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
