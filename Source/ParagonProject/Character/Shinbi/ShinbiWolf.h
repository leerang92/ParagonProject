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

	// Wolf State에 따라 행동할 함수 호출
	void Action(const EWolfState NewState, const FVector InitVec = FVector::ZeroVector, const FRotator InitRot = FRotator::ZeroRotator);

	// Attack Wolves 스킬 지속 시간
	UPROPERTY(EditAnywhere, Category = "Ability")
	float AttackWolvesDuration;

	/* 파티클 변수들 */
	// Attack Wolves 스킬 종료 시 생성할 이펙트
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* AttackWolvesEndFX;

	// Attack Wolves 스킬 타격 이펙트
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* AttackWolvesImpactFX;

	// Circling Wolves 스킬 타격 이펙트
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* CirclingWolvesImpactFX;

	// Circling Wolves 종료시 이펙트
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* CirclingRemovalFX;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* UltHeartFX;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* UltEyeFX;

	// 이펙트 생성 함수
	void SpawnParticle(UParticleSystem* NewFX);

	// 늑대 각도 변수 설정
	void SetCirclingAngle(float NewAngle);

	// Circling Wolves 스킬 중단
	void StopCirclingWolves();

	void SetTargetLocation(const FVector& TargetLoc);

	// 액터 활성화
	UFUNCTION(BlueprintCallable, Category = "Wolves")
	void SetEnable();
	
	// 액터 비활성화
	UFUNCTION(BlueprintCallable, Category = "Wolves")
	void SetDisable();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SetIsSpawn(bool bSpawn);

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* LeapMontage;

	void StartUltimate();

private:
	// 현재 상태에 맞는 함수를 업데이트하는 함수 포인터
	typedef void(AShinbiWolf::*fp)();
	fp UpdateFunc;

	// 현재 상태 변수
	EWolfState CurrentState;

	FRotator LookAtTarget(FVector MyLocation, FVector TargetLocation) const;

	/* Attack Wolves */
	// Attack Wolves 스킬 시작
	void StartAttackWolves();

	// Attack Wolves 스킬 종료
	void StopAttackWolves();

	// Attack Wolves 스킬 사용 여부
	bool IsAttackWolves : 1;

	// Attack Wolves 종료 타이머 핸들
	FTimerHandle AttackWolvesTimer;

	/* Circling Wolves */
	void StartCirclingWolves();

	// 타겟으로부터 회전할 포인트 반환 함수
	FVector RotateActorPoint(FVector TargetLocation, const float Radius, const float  Angle, const float RotationRate);

	// 플레이어부터의 늑대의 반경 (클수록 멀어짐)
	UPROPERTY(EditDefaultsOnly, Category = "Circling Wovles", meta = (AllowedPrivateAccess = "true"))
	float Radius;

	// 회전 속도
	UPROPERTY(EditDefaultsOnly, Category = "Circling Wovles", meta = (AllowedPrivateAccess = "true"))
	float RotSpeed;

	float UpdateRot;

	float StartAngle;

	bool bIsEnable : 1;

	/* Primary 스킬 */
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
