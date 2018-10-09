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
	/* Camera Variables */
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
	bool bSaveCombo;

	// Set the mouse position to center of the screen
	void SetMouseCenterLocation();

	bool bMovement;

	void ReigisterHitActor(AActor* HitActor);

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

	UPROPERTY(EditAnywhere, Category = "Component")
	class UAbilityComponent* AbilityComp;

	/* Attack */
	// Primary attack is montage section name array
	TArray<FName> PrimarySectionNames;

	bool bAttacking;

	int SectionCount;

	// Start primary attack
	virtual void StartPrimary();

	// Stop next attack 
	virtual void StopPrimary();

	// Reset current combo attack
	UFUNCTION(BlueprintCallable, Category = "Attack", meta = (AllowedPrivateAccess = true))
	virtual void ResetComboAttack();

	/* Ability */
	template<int T>
	void OnAbility();

	// Ability function pointer
	typedef void(ABaseCharacter::*AbilityFp)();
    TArray<AbilityFp> AbilityFuncs;

	virtual void AbilityMR();
	virtual void AbilityQ();
	virtual void AbilityE();
	virtual void Ultimate();

	void SetAbilityHUD(int Index);

	FTimerHandle ActiveAbilityTimer;

	virtual void ActiveAbility(int AbilityIndex);

	TArray<AActor*> HitActors;

	// Demage to nearby actors
	void OnRangeDamage(FVector& Origin, float BaseDamage, float DamageRadius);

	virtual void OnParticleToHitActor();

public:
	FORCEINLINE const UCameraComponent * GetCameraComponent()
	{
		return CameraComp;
	}

	FORCEINLINE const UAbilityComponent* GetAbilityComp()
	{
		return AbilityComp;
	}

	/* UI */
public:
	UPROPERTY(EditDefaultsOnly, Category = "UMG")
	TSubclassOf<UMainUMG> MainUMGClass;

protected:
	UPROPERTY()
	class UMainUMG* MainUMG;
};
