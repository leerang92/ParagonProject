// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShinbiWolf.generated.h"

UENUM(BlueprintType)
enum class EWolfState : uint8
{
	Idle,
	Attack,
	Circle,
	Primary,
};

UCLASS()
class PARAGONPROJECT_API AShinbiWolf : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShinbiWolf();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Wolf State�� ���� �ൿ�� �Լ� ȣ��
	void Action(const EWolfState NewState, const FVector InitVec = FVector::ZeroVector, const FRotator InitRot = FRotator::ZeroRotator);

	// Attack Wolves ��ų ���� �ð�
	UPROPERTY(EditAnywhere, Category = "Ability")
	float AttackWolvesDuration;

	/* ��ƼŬ ������ */
	// Attack Wolves ��ų ���� �� ������ ����Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* AttackWolvesEndFX;

	// Attack Wolves ��ų Ÿ�� ����Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* AttackWolvesImpactFX;

	// Circling Wolves ��ų Ÿ�� ����Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* CirclingWolvesImpactFX;

	// Circling Wolves ����� ����Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* CirclingRemovalFX;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* UltHeartFX;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* UltEyeFX;

	// ����Ʈ ���� �Լ�
	void SpawnParticle(UParticleSystem* NewFX);

	// ���� ���� ���� ����
	void SetCirclingAngle(float NewAngle);

	// Circling Wolves ��ų �ߴ�
	void StopCirclingWolves();

	void SetTargetLocation(const FVector& TargetLoc);

	// ���� Ȱ��ȭ
	UFUNCTION(BlueprintCallable, Category = "Wolves")
	void SetEnable();
	
	// ���� ��Ȱ��ȭ
	UFUNCTION(BlueprintCallable, Category = "Wolves")
	void SetDisable();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SetIsSpawn(bool bSpawn);

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* LeapMontage;

	void StartUltimate();

private:
	// ���� ���¿� �´� �Լ��� ������Ʈ�ϴ� �Լ� ������
	typedef void(AShinbiWolf::*fp)();
	fp UpdateFunc;

	// ���� ���� ����
	EWolfState CurrentState;

	FRotator LookAtTarget(FVector MyLocation, FVector TargetLocation) const;

	/* Attack Wolves */
	// Attack Wolves ��ų ����
	void StartAttackWolves();

	// Attack Wolves ��ų ����
	void StopAttackWolves();

	// Attack Wolves ��ų ��� ����
	bool IsAttackWolves : 1;

	// Attack Wolves ���� Ÿ�̸� �ڵ�
	FTimerHandle AttackWolvesTimer;

	/* Circling Wolves */
	void StartCirclingWolves();

	// Ÿ�����κ��� ȸ���� ����Ʈ ��ȯ �Լ�
	FVector RotateActorPoint(FVector TargetLocation, const float Radius, const float  Angle, const float RotationRate);

	// �÷��̾������ ������ �ݰ� (Ŭ���� �־���)
	UPROPERTY(EditDefaultsOnly, Category = "Circling Wovles", meta = (AllowedPrivateAccess = "true"))
	float Radius;

	// ȸ�� �ӵ�
	UPROPERTY(EditDefaultsOnly, Category = "Circling Wovles", meta = (AllowedPrivateAccess = "true"))
	float RotSpeed;

	float UpdateRot;

	float StartAngle;

	bool bIsEnable : 1;

	/* Primary ��ų */
	void UpdateUltimate();

	void StopUltimate();

	void SetupUltimate();

	FVector TargetLocation;

	bool bIsSpawn : 1;

private:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

public:
	UFUNCTION(BlueprintCallable, Category = "Shinbi Wolf")
	FORCEINLINE EWolfState GetState() const { return CurrentState; }

	FORCEINLINE bool IsEnable() const { return bIsEnable; }

};
