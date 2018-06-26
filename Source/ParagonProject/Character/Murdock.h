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

public:

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AMurdockProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	class UObjectPoolComponent* ObjPoolComp;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float ZeroDistance;

	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystem* HitPlayerFX;

	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystem* PlasmaFX;
	
private:
	virtual void StartAttack() override;

	virtual void Ability1() override;
};
