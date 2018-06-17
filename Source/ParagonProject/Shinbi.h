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
	// ������ ���� ��ü Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Wolf")
	TSubclassOf<class AShinbiWolf> WolfClass;

	// Dash ��ų ���ư� ũ�� ����
	UPROPERTY(EditAnywhere, Category = "Ability")
	float DashPower;

	// CirclingWolves(Abiility2) ���� �ð� ����
	UPROPERTY(EditAnywhere, Category = "Ability")
	float CirclingWolvesDuration;

	// �뽬 ��ų ���� ������ ��ƼŬ
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* DashParticle;

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

	// Attack Wolves ��ų ĳ��Ʈ �ߴ� �� ���� ��ȯ
	void StopAttackCast();

	// Circle Wolves ��ų �ߴ�
	void StopCircleWolves();

	// Circle Wolves ��ų �ߴ� Ÿ�̸��ڵ�
	FTimerHandle CircleWolvesTimer;

private:
	UPROPERTY(Transient)
	TArray<class AShinbiWolf*> Wolves;

	// Circling Wolves ��ų ���� ������ ���� �ε�����
	TSet<int> CirclingIndexes;

	bool bIsCircling : 1;

	// Circle Wolves ��ų ���� ���� ���� ����
	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (AllowedPrivateAccess = "true"))
	float WolfInterval;

	int WolfIndex;

	int MaxWolfNum;

	TSet<int> SetupWolves(const FVector SpawnVec, const FRotator SpawnRot, uint8 Type, int SpawnNum = 1);

	void CreateWolves();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
