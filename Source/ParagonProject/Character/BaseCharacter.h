// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Character/AbilityComponent.h"
#include "UI/MainUMG.h"
#include "BaseCharacter.generated.h"

UCLASS()
class PARAGONPROJECT_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Component")
	class UAbilityComponent* AbilityComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 현재 플레이어가 봐라보는 방향으로 트레이스 발사
	virtual APawn * FocusView();

	/* 이동 */
	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

public:
	/* 카메라 */
	UPROPERTY(EditAnywhere, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, Category = "View")
	float TraceDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* CameraComp;

	/* 카메라 파티클 */
	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystemComponent* CamParticle;

	// CamParticle 변수에 새 파티클 설정
	void SetCameraParticle(UParticleSystem* NewParticle);

public:
	/* 공격 */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TArray<UAnimMontage*> AttackMontages;

	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual void ComboAttack();

	UPROPERTY(BlueprintReadWrite, Category = "Attack")
	bool SaveCombo;

	// 마우스를 화면 중앙에 위치
	void SetMouseCenterLocation();

protected:
	/* Ability Animations */
	UPROPERTY(EditDefaultsOnly, Category = "Animation", meta = (AllowedPrivateAccess = true))
	UAnimMontage* PrimaryAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation", meta = (AllowedPrivateAccess = true))
	UAnimMontage* AbilityMRAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation", meta = (AllowedPrivateAccess = true))
	UAnimMontage* AbilityQAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation", meta = (AllowedPrivateAccess = true))
	UAnimMontage* AbilityEAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation", meta = (AllowedPrivateAccess = true))
	UAnimMontage* UltimateAnim;

protected:
	/* 공격 */
	// Primary 공격 몽타주 섹션 배열
	TArray<FName> PrimarySectionNames;

	bool bAttacking;

	int SectionCount;

	// 주 공격 시작
	virtual void StartPrimary();

	// 다음 콤보 어택 중단
	virtual void StopPrimary();

	// 현재 콤보 공격 초기화
	UFUNCTION(BlueprintCallable, Category = "Attack", meta = (AllowedPrivateAccess = true))
	virtual void ResetComboAttack();

	/* Ability */
	template<int T>
	void UseAbility();

	// Ability 함수 포인터 및 배열
	typedef void(ABaseCharacter::*AbilityFp)();
    TArray<AbilityFp> AbilityFuncs;

	virtual void AbilityMR();
	virtual void AbilityQ();
	virtual void AbilityE();
	virtual void Ultimate();

public:
	inline const UCameraComponent * GetCameraComponent()
	{
		return CameraComp;
	}

	/* UI */
public:
	UPROPERTY(EditDefaultsOnly, Category = "UMG")
	TSubclassOf<UMainUMG> MainUMGClass;

protected:
	UPROPERTY()
	class UMainUMG* MainUMG;

	void SetAbilityBar();
};
