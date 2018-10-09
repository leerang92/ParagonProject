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

	// Heaven's Fury �ɷ� ��� �� ������ ������ ���� ����
	void OnHeavenFuryHover();

	void StopHeavenFury();

	// Ascend ����� ���� ĳ���� �̵�
	void MovementAscendAbility(const float DeltaTime);

	// ī�޶󿡼����� ���콺 ��ġ�� Ʈ���̽� �Ÿ� ��ȯ
	float GetTraceHitDistance(const FVector& NewPos, const FVector& NewDir);

	// ���콺�� ���� ������ ��ȯ
	FVector GetMouseWorldPosition();

	// Ascend ��� ���� ���� ���ư� ��ǥ ��ġ
	FVector FlyTargetVec;

	// Ascend ��� ���� Ÿ�� ��Į ��ġ
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
