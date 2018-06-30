// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Serath.generated.h"

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

protected:
	virtual void StartPrimary() override;

	UFUNCTION(BlueprintCallable, Category = "Serath", meta = (AllowedPrivatedAccess = true))
	virtual void ComboAttack() override;

	UFUNCTION(BlueprintCallable, Category = "Serath", meta = (AllowedPrivatedAccess = true))
	virtual void ResetComboAttack() override;

public:

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* WingFlapAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* FuryAnim;

	UFUNCTION(BlueprintCallable, Category = "Serath")
	void SetFly(bool bIsFly);

	UPROPERTY(EditAnywhere, Category = "Speed")
	float FlySpeed;

	UPROPERTY(EditAnywhere)
	class UMaterial* Decal;

private:
	EAbilityType CurrentAbility;

	// Primary 공격 몽타주 섹션 배열
	TArray<FName> PrimarySectionNames;

	int SectionCount;

	bool bAbilityFury : 1;

	bool bFly : 1;

	virtual void AbilityMR() override;

	virtual void AbilityQ() override;

	virtual void AbilityE() override;

	void SetHeavenFury();

	FVector FlyTargetVec;

	float GetTraceHitDistance(const FVector& NewPos, const FVector& NewDir);

	FVector GetMouseWorldPosition();

	UPROPERTY()
	class UDecalComponent* DecalComp;

	UPROPERTY()
	class APlayerController* CharCon;

public:
	UFUNCTION(BlueprintCallable, Category = "Character")
	FORCEINLINE EAbilityType GetCurrentAbility() const { return CurrentAbility; }
	
};
