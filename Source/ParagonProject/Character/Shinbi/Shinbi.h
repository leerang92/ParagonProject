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
	// ������ ���� ��ü Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Wolf")
	TSubclassOf<class AShinbiWolf> WolfClass;

	// ������ ���� ��
	UPROPERTY(EditDefaultsOnly, Category = "Wolves")
	int MaxWolfNum;

	// Dash ��ų ���ư� ũ�� ����
	UPROPERTY(EditAnywhere, Category = "Ability")
	float DashPower;

	// CirclingWolves(Abiility2) ���� �ð� ����
	UPROPERTY(EditAnywhere, Category = "Ability")
	float CirclingWolvesDuration;

	// Primary ��ų �� ������� ���� ����
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float UltimateAngleInterval;

	// Primary ��ų ���� ���� �ð�
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float UltimateSpawnTime;

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

	/* Attack */
	virtual void StartAttack() override;

	virtual void ComboAttack() override;

	FTimerHandle AttackTimer;

	/* Ability */
	virtual void Ultimate() override;
	virtual void Ability1() override;
	virtual void Ability2() override;
	virtual void AbilityMouseR() override;

	// Attack Wolves ��ų ĳ��Ʈ �ߴ� �� ���� ��ȯ
	void StopAttackCast();

	// Circle Wolves ��ų �ߴ�
	void StopCircleWolves();

	/* Primary ��ų */
	void SetUltimateWolves(int Index);

	FVector SetAngle(FVector NewLocation, float NewAngle);
private:
	UPROPERTY(Transient)
	TArray<class AShinbiWolf*> Wolves;

	// Circling Wolves ��ų ���� ������ ���� �ε�����
	TSet<int> CirclingIndexes;

	// Circle Wolves ��ų ���� ���� ���� ����
	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (AllowedPrivateAccess = "true"))
	float WolfInterval;

	bool bIsCircling : 1;

	bool bIsUltimate : 1;

	// Circle Wolves ��ų �ߴ� Ÿ�̸��ڵ�
	FTimerHandle CircleWolvesTimer;

	// Primary ��ų ���� ���� Ÿ�̸��ڵ�
	FTimerHandle SpawnWolvesTimer[8];

	// ���� ���� �迭�� �ε���
	int WolfIndex;

	// ���� Ȱ��ȭ �Լ�
	TSet<int> SetupWolves(const FVector SpawnVec, const FRotator SpawnRot, uint8 Type, int SpawnNum = 1);

	// ���� ���۽� ���� ����
	void CreateWolves();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
