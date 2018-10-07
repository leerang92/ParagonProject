// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseMinion.h"
#include "Character/BaseCharacter.h"


// Sets default values
ABaseMinion::ABaseMinion()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseMinion::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseMinion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABaseMinion::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(DamageCauser);
	if (Character != nullptr)
	{
		Character->ReigisterHitActor(this);
	}

	return Damage;
}

