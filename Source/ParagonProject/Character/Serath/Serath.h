// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Serath.generated.h"


UENUM()
enum class EAscendState : uint8
{
	None,
	Intro,
	Fly,
	Dive
};
/**
 * 
 */
UCLASS()
class PARAGONPROJECT_API ASerath : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	ASerath();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SetFly();

	void OnFuryDamage();

	virtual void ComboAttack() override;

	virtual void ResetComboAttack() override;

protected:
	virtual void StartPrimary() override;

	virtual void OnParticleToHitActor() override;

public:
	UPROPERTY(EditAnywhere)
	class UMaterial* Decal;

	/* Paritcle */
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* AscendCamFX;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* AscendDiveFX;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* FuryImpactFX;

	UPROPERTY(EditAnywhere, Category = "Ascend")
	float AscendFlySpeed;

	UPROPERTY(EditAnywhere, Category = "Ascend")
	float AscendDiveSpeed;

private:
	EAscendState CurrentAscendState;

	EAbilityType CurrentAbilityType;

	bool bAscend : 1;

	bool bFury : 1;

	/* Ability */
	// Chastise
	virtual void AbilityMR() override;

	// Heaven's Fury
	virtual void AbilityQ() override;

	// Ascend
	virtual void AbilityE() override;

	// Heaven's Fury 능력 사용 중 선택한 범위에 공격 적용
	void OnHeavenFuryHover();

	void StopHeavenFury();

	// Ascend 기술에 따른 캐릭터 이동
	void MovementAscendAbility(const float DeltaTime);

	// 카메라에서부터 마우스 위치로 트레이스 거리 반환
	float GetTraceHitDistance(const FVector& NewPos, const FVector& NewDir);

	// 마우스의 월드 포지션 반환
	FVector GetMouseWorldPosition();

	// Ascend 기술 사용시 위로 날아갈 목표 위치
	FVector FlyTargetVec;

	// Ascend 기술 사용시 타겟 데칼 위치
	FVector AscendDecalLoc;

	UPROPERTY()
	class UDecalComponent* DecalComp;

	UPROPERTY()
	class APlayerController* CharCon;

	UPROPERTY()
	class UParticleSystemComponent* AscendFXComp;

	FTimerHandle WaitTimer;

public:
	UFUNCTION(BlueprintCallable, Category = "Player Character")
	FORCEINLINE EAbilityType GetCurrentAbility() const { return CurrentAbilityType; }
};
