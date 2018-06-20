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

	// ���� �÷��̾ ���󺸴� �������� Ʈ���̽� �߻�
	virtual APawn * FocusView();

	/* �̵� */
	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

public:
	/* ī�޶� */
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

	/* ī�޶� ��ƼŬ */
	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystemComponent* CamParticle;

	// CamParticle ������ �� ��ƼŬ ����
	void SetCameraParticle(UParticleSystem* NewParticle);

public:
	/* ���� */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TArray<UAnimMontage*> AttackMontages;

	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual void ComboAttack();

	UPROPERTY(BlueprintReadWrite, Category = "Attack")
	bool SaveCombo;

protected:
	/* ���� */
	bool IsAttacking : 1;

	int AttackCount;

	virtual void StartAttack();

	virtual void StopAttack();

	// ���� �޺� ���� �ʱ�ȭ
	UFUNCTION(BlueprintCallable, Category = "Attack", meta = (AllowedPrivateAccess = true))
	virtual void ResetComboAttack();

	/* Ability */
	template<int T>
	void UseAbility();

	// Ability �Լ� ������ �� �迭
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
