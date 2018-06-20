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
	Ultimate,
};

UCLASS()
class PARAGONPROJECT_API AShinbiWolf : public ACharacter
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	AShinbiWolf();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Init(class AShinbi* Owner);

	// Wolf State�� ���� �ൿ�� �Լ� ȣ��
	void Action(const EWolfState NewState);

	// Attack Wolves ��ų ���� �ð�
	UPROPERTY(EditAnywhere, Category = "Ability")
	float AttackWolvesDuration;

	UPROPERTY(EditAnywhere, Category = "Ability")
	float CirclWolvesDuration;

public:
	/* ��ƼŬ ������ */
	// Attack Wolves ��ų ���� �� ������ ����Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "Attack Particle")
	UParticleSystem* AttackWolvesEndFX;

	// Attack Wolves ��ų Ÿ�� ����Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "Attack Particle")
	UParticleSystem* AttackWolvesImpactFX;

	// Circling Wolves ��ų Ÿ�� ����Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "Circling Particle")
	UParticleSystem* CirclingWolvesImpactFX;

	// Circling Wolves ����� ����Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "Circling Particle")
	UParticleSystem* CirclingRemovalFX;

	UPROPERTY(EditDefaultsOnly, Category = "Ultimate Particle")
	UParticleSystem* UltHeartFX;

	UPROPERTY(EditDefaultsOnly, Category = "Ultimate Particle")
	UParticleSystem* UltEyeFX;

	UPROPERTY(EditDefaultsOnly, Category = "Ultimate Particle")
	UParticleSystem* UltEndFX;

	UPROPERTY(EditDefaultsOnly, Category = "Ultimate Particle")
	UParticleSystem* UltFinalFX;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* LeapMontage;

public:
	// ���� Ȱ��ȭ
	UFUNCTION(BlueprintCallable, Category = "Wolves")
	void SetEnable();
	
	// ���� ��Ȱ��ȭ
	UFUNCTION(BlueprintCallable, Category = "Wolves")
	void SetDisable();

	// ���� ���� ���� ����
	void SetCirclingAngle(float NewAngle);

	// Circling Wolves ��ų �ߴ�
	void StopCirclingWolves();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SetIsSpawn(bool bSpawn);

	// Ultimate ��ų ����
	void SetupUltimate(APawn* Target, const FVector NewLocation, float InRate);

	void SetUltimateFinal(bool bFinal);

private:
	UPROPERTY()
	class AShinbi* OwnerPawn;

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

	FTimerHandle CirRemoveTimer;

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

	/* Ultimate ��ų */
	FTimerHandle UltTimer;

	APawn* TargetPawn;

	bool bIsSpawn : 1;

	void UpdateUltimate();

	void StopUltimate();

	bool bIsFinal : 1;

	// ����Ʈ ���� �Լ�
	void SpawnParticle(UParticleSystem* NewFX);

	void StartUltimate();

private:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

public:
	UFUNCTION(BlueprintCallable, Category = "Shinbi Wolf")
	FORCEINLINE EWolfState GetState() const { return CurrentState; }
};
