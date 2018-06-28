// Fill out your copyright notice in the Description page of Project Settings.

#include "Murdock.h"
#include "Character/MurdockProjectile.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

AMurdock::AMurdock()
{
	ZeroDistance = 2000.0f;

	SprintSpeed = 1200.0f;
	HotPursuitDuration = 5.0f;

	ObjPoolComp = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("Object Pool Component"));
}

void AMurdock::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileClass)
	{
		ObjPoolComp->CreateObject(GetWorld(), ProjectileClass, FVector::ZeroVector, GetActorRotation(), 10);
		check(ObjPoolComp);
	}
}

void AMurdock::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/* 스킬 */
	PlayerInputComponent->BindAction("AbilityMouseR", IE_Released, this, &AMurdock::StartShotgun);
	PlayerInputComponent->BindAction("Ability1", IE_Released, this, &AMurdock::StopShield);
}

void AMurdock::StartAttack()
{
		// 공격중이 아닐 시
	if (!bAttacking)
	{
		bAttacking = true;
		SaveCombo = true;
		ComboAttack();
	}
	// 이미 공격중일 시 콤보 이어하기
	else if (!SaveCombo)
	{
		SaveCombo = true;
	}
}

void AMurdock::ComboAttack()
{
	if (SaveCombo && AttackMontages.Num() > 0)
	{
		Super::ComboAttack();

		PlayAnimMontage(AttackMontages[0]);

		const FVector CamLoc = CameraComp->GetComponentLocation();
		const FRotator CamRot = CameraComp->GetComponentRotation();
		const FVector MuzzleLoc = GetMesh()->GetSocketLocation(TEXT("Muzzle"));

		const FVector ZeroDir = CamRot.RotateVector(FVector::ForwardVector);
		const FVector ZeroPoint = (ZeroDir * ZeroDistance) + CamLoc;
		const FVector ProjectileDir = (ZeroPoint - MuzzleLoc).GetSafeNormal();

		// 트레이스 발사
		FCollisionQueryParams TraceParam(TEXT("TracePlayerView"), true, this);
		TraceParam.bTraceAsyncScene = true;
		TraceParam.bReturnPhysicalMaterial = false;
		TraceParam.bTraceComplex = true;

		FHitResult Hit(ForceInit);
		GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLoc, ZeroPoint, ECC_Pawn, TraceParam);
		FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(MuzzleLoc, Hit.ImpactPoint);

		AMurdockProjectile* Projectile = ObjPoolComp->ActiveObject<AMurdockProjectile>();
		Projectile->SetActorLocation(MuzzleLoc);
		Projectile->SetActorRotation(LookRot);
		Projectile->FireDirection(ProjectileDir);
	}
}

// Spread Shot
void AMurdock::AbilityMouseR()
{
	Super::AbilityMouseR();

	PlayAnimMontage(AttackMontages[1]);
}

// Gun Shield
void AMurdock::Ability1()
{
	Super::Ability1();

	CurrentAbility = EAbilityType::Ability1;

	// 쉴드 애니메이션 재생 및 이펙트 생성
	PlayAnimMontage(AttackMontages[2]);
	ShieldComp = UGameplayStatics::SpawnEmitterAttached(ShieldFX, GetMesh(), TEXT("Shield"));
}

// Hot Pusuit
void AMurdock::Ability2()
{
	Super::Ability2();

	bHotPursuit = true;

	// 이동속도 변경
	const float WalkSpeed = GetCharacterMovement()->GetMaxSpeed();
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	// Hot Pursuit 종료 타이머
	FTimerDelegate HotpursuitDel = FTimerDelegate::CreateUObject(this, &AMurdock::StopHotPursuit, WalkSpeed);
	GetWorldTimerManager().SetTimer(HotPursuitTimer, HotpursuitDel, HotPursuitDuration, false);
}

void AMurdock::StartShotgun()
{
	PlayAnimMontage(AttackMontages[1], 1.0f, TEXT("End"));
}

void AMurdock::StopShield()
{
	CurrentAbility = EAbilityType::Primary;
	ShieldComp->Deactivate();
	PlayAnimMontage(AttackMontages[2], 1.0f, TEXT("End"));
}

void AMurdock::ShieldHitStart()
{
	PlayAnimMontage(AttackMontages[2], 1.0f, TEXT("Hit"));
}

void AMurdock::ShieldHitEnd()
{
	PlayAnimMontage(AttackMontages[2], 1.0f, TEXT("Loop"));
}

void AMurdock::StopHotPursuit(float WalkSpeed)
{
	bHotPursuit = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
