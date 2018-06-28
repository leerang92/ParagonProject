// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MurdockProjectile.generated.h"

UCLASS()
class PARAGONPROJECT_API AMurdockProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMurdockProjectile();

	UPROPERTY(EditAnywhere, Category = "Projectile")
	class UProjectileMovementComponent* ProjectileComp;

	UPROPERTY(EditAnywhere, Category = "Collision")
	class UCapsuleComponent* CapsuleCol;

	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystemComponent* FXComp;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* PlasmaFX;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* HitPlayerFX;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* HitWorldFX;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void FireDirection(const FVector& Direction);

private:
	UPROPERTY()
	class ACharacter* OwnerPawn;

	FTimerHandle DestroyTimer;

	void SetDestroy();
	
};
