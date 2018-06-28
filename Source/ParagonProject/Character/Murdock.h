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
	// 발사체 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AMurdockProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	class UObjectPoolComponent* ObjPoolComp;

	// 영점 거리
	UPROPERTY(EditAnywhere, Category = "Projectile")
	float ZeroDistance;

	// Ability Hot Pursuit시 이동 속도
	UPROPERTY(EditAnywhere, Category = "Ability HotPursuit")
	float SprintSpeed;

	// Ability Hot Pursuit 지속 시간
	UPROPERTY(EditAnywhere, Category = "Ability HotPursuit")
	float HotPursuitDuration;

public:
	/* 파티클 변수들 */
	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystem* HitPlayerFX;

	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystem* PlasmaFX;

	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystem* ShieldFX;

	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystem* ShieldDetecFX;

private:
	bool bHotPursuit : 1;

	// 현재 어빌리티
	EAbilityType CurrentAbility;

private:

	virtual void StartAttack() override;

	// Spread Shotgun Ability
	virtual void AbilityMouseR() override;

	// Gun Shield Ability
	virtual void Ability1() override;

	// Hot Pusuit Ability
	virtual void Ability2() override;

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

public:
	UFUNCTION(BlueprintCallable, Category = "Murdock")
	FORCEINLINE bool IsHotPursuit() const { return bHotPursuit; }

	UFUNCTION(BlueprintCallable, Category = "Murdock")
	FORCEINLINE EAbilityType GetAbilityType() const { return CurrentAbility; }
};
