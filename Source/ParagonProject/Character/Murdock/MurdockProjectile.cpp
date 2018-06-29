// Fill out your copyright notice in the Description page of Project Settings.

#include "MurdockProjectile.h"
#include "Character/Murdock/Murdock.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"


// Sets default values
AMurdockProjectile::AMurdockProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CapsuleCol = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	CapsuleCol->SetCapsuleRadius(10.0f);
	CapsuleCol->SetCapsuleHalfHeight(10.0f);
	CapsuleCol->BodyInstance.SetCollisionProfileName(TEXT("Proejctile"));
	CapsuleCol->OnComponentBeginOverlap.AddDynamic(this, &AMurdockProjectile::BeginOverlap);
	RootComponent = CapsuleCol;

	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Compnent"));
	ProjectileComp->SetUpdatedComponent(CapsuleCol);
	ProjectileComp->MaxSpeed = 5000.0f;
	ProjectileComp->bRotationFollowsVelocity = true;
	ProjectileComp->bShouldBounce = true;
	ProjectileComp->Bounciness = 0.3f;
	ProjectileComp->ProjectileGravityScale = 0.0f;

	FXComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Paritcle System Component"));
	FXComp->AttachTo(CapsuleCol);
}

// Called when the game starts or when spawned
void AMurdockProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerPawn = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void AMurdockProjectile::BeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr
		&& OtherActor != OwnerPawn)
	{
		AMurdock* Player = Cast<AMurdock>(OwnerPawn);
		if (Player)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitPlayerFX, GetActorTransform());
		}
		else
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitWorldFX, GetActorTransform());
		}
		Player->ObjPoolComp->SetDisable(this);
	}
}

// Called every frame
void AMurdockProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMurdockProjectile::FireDirection(const FVector & Direction)
{
	ProjectileComp->Velocity = Direction * ProjectileComp->MaxSpeed;
	FXComp->SetTemplate(PlasmaFX);
	//UGameplayStatics::SpawnEmitterAttached(PlasmaFX, CapsuleCol);
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AMurdockProjectile::SetDestroy, 2.0f, false);
}

void AMurdockProjectile::SetDestroy()
{
	AMurdock* Player = Cast<AMurdock>(OwnerPawn);
	if (Player)
	{
		Player->ObjPoolComp->SetDisable(this);
	}
}

