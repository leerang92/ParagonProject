// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Character/AbilityComponent.h"
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

protected:
	/* 공격 */
	bool IsAttacking : 1;

	int AttackCount;

	virtual void StartAttack();

	virtual void StopAttack();

	// 현재 콤보 공격 초기화
	UFUNCTION(BlueprintCallable, Category = "Attack", meta = (AllowedPrivateAccess = true))
	virtual void ResetComboAttack();

	/* Ability */
	template<int T>
	void UseAbility();

	// Ability 함수 포인터 및 배열
	typedef void(ABaseCharacter::*AbilityFp)();
    TArray<AbilityFp> AbilityFuncs;

	virtual void AbilityMouseR();
	virtual void Ability1();
	virtual void Ability2();
	virtual void Ultimate();

protected:
	inline const UCameraComponent * GetCameraComponent()
	{
		return CameraComp;
	}
};
