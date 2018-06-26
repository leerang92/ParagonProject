// Fill out your copyright notice in the Description page of Project Settings.

#include "Murdock.h"
#include "Character/MurdockProjectile.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

AMurdock::AMurdock()
{
	ZeroDistance = 5000.0f;

	ObjPoolComp = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("Object Pool Component"));
}

void AMurdock::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileClass)
	{
		ObjPoolComp->CreateObject(GetWorld(), ProjectileClass, FVector::ZeroVector, GetActorRotation(), 10);
	}
}

void AMurdock::StartAttack()
{
	PlayAnimMontage(AttackMontages[0]);

	const FVector CamLoc = CameraComp->GetComponentLocation();
	const FRotator CamRot = CameraComp->GetComponentRotation();
	const FVector MuzzleLoc = GetMesh()->GetSocketLocation(TEXT("Muzzle"));

	const FVector ZeroDir = CamRot.RotateVector(FVector::ForwardVector);
	const FVector ZeroPoint = (ZeroDir * ZeroDistance) + CamLoc;
	const FVector ProjectileDir = (ZeroPoint - MuzzleLoc).GetSafeNormal();

	AMurdockProjectile* Projectile = ObjPoolComp->ActiveObject<AMurdockProjectile>();
	Projectile->SetActorLocation(GetActorLocation());
	Projectile->FireDirection(ProjectileDir);

	// 트레이스 발사
	//FCollisionQueryParams TraceParam(TEXT("TracePlayerView"), true, this);
	//TraceParam.bTraceAsyncScene = true;
	//TraceParam.bReturnPhysicalMaterial = false;
	//TraceParam.bTraceComplex = true;

	//FHitResult Hit(ForceInit);
	//GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLoc, ZeroPoint, ECC_Pawn, TraceParam);

	//if (Hit.Actor != nullptr && HitPlayerFX != nullptr)
	//{
	//	FTransform NewTr = FTransform(Hit.Normal.Rotation() * -0.1f, Hit.Location);
	//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitPlayerFX, NewTr);
	//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PlasmaFX, NewTr);
	//	UE_LOG(LogClass, Warning, TEXT("Hit Actor : %s"), *Hit.Actor->GetName());
	//}

	DrawDebugLine(GetWorld(), MuzzleLoc, ZeroPoint, FColor::Red, false, 0.5f, 3.0f);
}

void AMurdock::Ability1()
{
	PlayAnimMontage(AttackMontages[1]);
}
