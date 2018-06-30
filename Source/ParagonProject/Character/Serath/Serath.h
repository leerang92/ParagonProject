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

	UFUNCTION(BlueprintCallable, Category = "Serath")
	void SetFly();

protected:
	virtual void StartPrimary() override;

	UFUNCTION(BlueprintCallable, Category = "Serath", meta = (AllowedPrivatedAccess = true))
	virtual void ComboAttack() override;

	UFUNCTION(BlueprintCallable, Category = "Serath", meta = (AllowedPrivatedAccess = true))
	virtual void ResetComboAttack() override;

public:

	/* Animation */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* WingFlapAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* FuryAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* AscendAnim;

	/* Paritcle */
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* AscendCamFX;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	class UParticleSystem* AscendDiveFX;

	UPROPERTY(EditAnywhere, Category = "Ascend")
	float AscendFlySpeed;

	UPROPERTY(EditAnywhere, Category = "Ascend")
	float AscendDiveSpeed;

	// 데칼 마테리얼
	UPROPERTY(EditAnywhere)
	class UMaterial* Decal;

private:
	EAbilityType CurrentAbility;

	EAscendState CurrentAscend;

	// Primary 공격 몽타주 섹션 배열
	TArray<FName> PrimarySectionNames;

	int SectionCount;

	/* Ability */
	virtual void AbilityMR() override;

	virtual void AbilityQ() override;

	virtual void AbilityE() override;

	void SetHeavenFury();

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

public:
	UFUNCTION(BlueprintCallable, Category = "Character")
	FORCEINLINE EAbilityType GetCurrentAbility() const { return CurrentAbility; }
	
};
