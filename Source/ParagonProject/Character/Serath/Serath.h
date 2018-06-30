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

	// ��Į ���׸���
	UPROPERTY(EditAnywhere)
	class UMaterial* Decal;

private:
	EAbilityType CurrentAbility;

	EAscendState CurrentAscend;

	// Primary ���� ��Ÿ�� ���� �迭
	TArray<FName> PrimarySectionNames;

	int SectionCount;

	/* Ability */
	virtual void AbilityMR() override;

	virtual void AbilityQ() override;

	virtual void AbilityE() override;

	void SetHeavenFury();

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

public:
	UFUNCTION(BlueprintCallable, Category = "Character")
	FORCEINLINE EAbilityType GetCurrentAbility() const { return CurrentAbility; }
	
};
