// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/CharacterTypes.h"
#include "AbilityComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARAGONPROJECT_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityComponent();

	UPROPERTY(EditAnywhere, Category = "UMG")
	TSubclassOf<class UUIAbilityBar> AbilityBarClass;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	void UseAbility(const EAbilityType Type);

	FAbilityInfo GetAbilityInfo(int Index) const;

private:
	void SetPrimary();

	void SetMouseR();

	void SetAbility1();

	void SetAbility2();

	void SetUltimate();

	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (AllowedPrivateAccess = true))
	FAbilityInfo AbilityInfo[5];

	class UUIAbilityBar* AbilityBar;

};
