// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/ObjectPoolComponent.h"
#include "Murdock.generated.h"

/**
 * 
 */
UCLASS()
class PARAGONPROJECT_API AMurdock : public ABaseCharacter
{
	GENERATED_BODY()

private:
	AMurdock();

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual void ComboAttack() override;

public:
	// �߻�ü Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AMurdockProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	class UObjectPoolComponent* ObjPoolComp;

	// ���� �Ÿ�
	UPROPERTY(EditAnywhere, Category = "Projectile")
	float ZeroDistance;

	// Ability Hot Pursuit�� �̵� �ӵ�
	UPROPERTY(EditAnywhere, Category = "Ability HotPursuit")
	float SprintSpeed;

	// Ability Hot Pursuit ���� �ð�
	UPROPERTY(EditAnywhere, Category = "Ability HotPursuit")
	float HotPursuitDuration;

public:
	/* ��ƼŬ ������ */
	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystem* HitPlayerFX;

	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystem* PlasmaFX;

	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystem* ShieldFX;

	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystem* ShieldDetecFX;

	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystem* HotLightFX;

private:
	bool bHotPursuit : 1;

	// ���� �����Ƽ
	EAbilityType CurrentAbility;

private:

	virtual void StartPrimary() override;

	// Spread Shotgun Ability
	virtual void AbilityMR() override;

	// Gun Shield Ability
	virtual void AbilityQ() override;

	// Hot Pusuit Ability
	virtual void AbilityE() override;

	void StartShotgun();

	void StopShield();

	void ShieldHitStart();

	UFUNCTION(BlueprintCallable, Category = "Murdock", meta = (AllowedPrivateAccess = true))
	void ShieldHitEnd();

	FTimerHandle HotPursuitTimer;

	void StopHotPursuit(float WalkSpeed);

private:
	UPROPERTY()
	UParticleSystemComponent * ShieldComp;

	UPROPERTY()
	UParticleSystemComponent* HotpursuitFXComp;

public:
	UFUNCTION(BlueprintCallable, Category = "Murdock")
	FORCEINLINE bool IsHotPursuit() const { return bHotPursuit; }

	UFUNCTION(BlueprintCallable, Category = "Murdock")
	FORCEINLINE EAbilityType GetAbilityType() const { return CurrentAbility; }
};
