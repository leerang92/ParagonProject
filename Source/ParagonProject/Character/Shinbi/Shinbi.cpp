// Fill out your copyright notice in the Description page of Project Settings.

#include "Shinbi.h"
#include "ShinbiWolf.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AShinbi::AShinbi()
{
	PrimaryActorTick.bCanEverTick = false;

	/* 늑대 오브젝트 풀 */
	MaxWolfNum = 20;
	WolfIndex = 0;

	DashPower = 4500.0f;

	/* Circling Wolves 스킬 */
	CircleWolfRate = 90.0f;
	CirclingWolvesDuration = 7.0f;

	/* Primary 스킬 */
	UltimateAngleInterval = 60.0f;
	UltimateSpawnTime = 0.5f;
	UltHitCount = 0;
}

void AShinbi::BeginPlay()
{
	Super::BeginPlay();

	// 늑대 액터 생성(오브젝트풀)
	CreateWolves();
}

void AShinbi::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("AbilityMouseR", IE_Released, this, &AShinbi::StopAttackCast);
}

void AShinbi::StartAttack()
{
	// 공격중이 아닐 시
	if (!IsAttacking)
	{
		IsAttacking = true;
		SaveCombo = true;
		ComboAttack();
	}
	// 이미 공격중일 시 콤보 이어하기
	else if (!SaveCombo)
	{
		SaveCombo = true;
	}
}

void AShinbi::ComboAttack()
{
	if (SaveCombo && AttackMontages.Num() > 0)
	{
		// 공격 몽타주 애니메이션 재생
		PlayAnimMontage(AttackMontages[AttackCount++]);

		// 마지막 공격이면 공격 카운트 초기화
		if (AttackMontages.Num() <= AttackCount)
		{
			AttackCount = 0;
		}
	}
}

// Attack Wolves 스킬
void AShinbi::AbilityMouseR()
{
	// Attack Wolves 스킬 애니메이션 몽타주 실행
	PlayAnimMontage(AttackWolvesMontage);
}

void AShinbi::StopAttackCast()
{
	// 애님 몽타주의 End 섹션 재생
	PlayAnimMontage(AttackWolvesMontage, 1.0f, TEXT("End"));

	// 울프 액터 생성
	const FVector SpawnVec = GetActorLocation() + (GetActorForwardVector() * FVector(0, 0, -6.0f));
	TSet<AShinbiWolf*> WolfSet = ActiveWolves(SpawnVec, GetActorRotation(), 1);
	for (auto& wolf : WolfSet)
	{
		wolf->Action(EWolfState::Attack);
	}
}

// Dash
void AShinbi::Ability1()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		PlayAnimMontage(DashMontage, 2.0f);
		SetCameraParticle(DashParticle); // 카메라에 대쉬 이펙트 실행

		// 정면으로 대쉬
		LaunchCharacter(GetActorForwardVector() * DashPower, true, true);
	}
}

// Circling Wolves 스킬
void AShinbi::Ability2()
{
	if (bIsCircling)
	{
		return;
	}

	// 플레이어 CircleWovles 애니메이션 실행
	PlayAnimMontage(CircleWolvesMontage);
	// 늑대들 생성
	CirclingWolfSet = ActiveWolves(GetActorLocation(), GetActorRotation(), 4);
	// 늑대들 각도 설정
	float Angle = 0.0f;
	for (auto wolf : CirclingWolfSet)
	{
		wolf->SetCirclingAngle(Angle);
		wolf->Action(EWolfState::Circle);
		Angle += CircleWolfRate;
	}
	// CirclingWolvesDuration 시간 후 울프 객체들 사라짐
	//GetWorldTimerManager().SetTimer(CircleWolvesTimer, this, &AShinbi::StopCircleWolves, CirclingWolvesDuration, false);

	bIsCircling = true;
}

void AShinbi::StopCircleWolves()
{
	for (auto wolf : CirclingWolfSet)
	{
		// 제거 이펙트 생성 및 Disable
		wolf->StopCirclingWolves();
	}
	CirclingWolfSet.Empty();
	bIsCircling = false;
}

void AShinbi::Ultimate()
{
	TargetPawn = FocusView();
	if (TargetPawn != nullptr)
	{
		// 타겟에게 마커 이펙트 생성
		MarkerFX = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), UltMarker, TargetPawn->GetActorTransform());

		// 애니메이션 실행
		PlayAnimMontage(UltMontage);

		// 타겟과의 각도 구함
		FVector Dir = TargetPawn->GetActorLocation() - GetActorLocation();
		float Angle = FMath::Atan2(Dir.Z, Dir.X);
		Angle = FMath::RadiansToDegrees(Angle);
		Angle += 130.0f;

		float TimeRate = 0.6f;
		// 늑대 생성
		TSet<AShinbiWolf*> WolfSet = ActiveWolves(FVector::ZeroVector, FRotator::ZeroRotator, 8);
		for (auto& wolf : WolfSet)
		{
			// 늑대 위치, 상태 설정
			const FVector SpawnLoc = SetAngle(TargetPawn->GetActorLocation(), Angle);
			wolf->SetupUltimate(TargetPawn, SpawnLoc, TimeRate);
			wolf->Action(EWolfState::Ultimate);

			TimeRate += 0.15f;
			Angle += 15.0f;
		}
	}
	GetWorldTimerManager().SetTimer(MarkerTimer, this, &AShinbi::DestroyMarker, 0.6f, false);
}

void AShinbi::DestroyMarker()
{
	MarkerFX->Deactivate();
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), UltMarkerDestroy, TargetPawn->GetActorTransform());
}

void AShinbi::UltimateHitNotify()
{
	++UltHitCount;

	if (UltHitCount >= 8)
	{
		UltHitCount = 0;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), UltFinalFX, TargetPawn->GetActorTransform());
		GetWorldTimerManager().SetTimer(UltShakeTimer, this, &AShinbi::CameraShake, 0.7f, false);
	}
}

void AShinbi::CameraShake()
{
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), CamShake, GetActorLocation(), 0.0f, 500.0f);
}

FVector AShinbi::SetAngle(FVector NewLocation, float NewAngle)
{
	/* 타겟을 중심으로 PrimaryAngle만큼 주위 위치값 구함 */
	const FVector RotationVec = FVector(400.0f, 0, 140.0f);
	const FVector Axis = FVector(0, 0, 1).GetSafeNormal();
	const FVector ResultVec = RotationVec.RotateAngleAxis(NewAngle, Axis);
	NewLocation += ResultVec;

	return NewLocation;
}

TSet<AShinbiWolf*> AShinbi::ActiveWolves(const FVector SpawnVec, const FRotator SpawnRot, int SpawnNum)
{
	TSet<AShinbiWolf*> WolfSet;
	// 현재 사용중이지 않은 늑대 액터 설정
	for (int SpawnCount = 0; SpawnCount < SpawnNum;)
	{
		if (!Wolves[WolfIndex]->IsHiddenEd())
		{
			Wolves[WolfIndex]->SetEnable();
			Wolves[WolfIndex]->SetActorTransform(FTransform(SpawnRot, SpawnVec));
			WolfSet.Add(Wolves[WolfIndex]);
			++SpawnCount;
		}
		// 인덱스 순환
		++WolfIndex %= MaxWolfNum;
	}
	return WolfSet;
}

void AShinbi::CreateWolves()
{
	for (int i = 0; i < MaxWolfNum; ++i)
	{
		Wolves.Add(GetWorld()->SpawnActor<AShinbiWolf>(WolfClass, GetActorLocation(), GetActorRotation()));
		Wolves[i]->Init(this);
		Wolves[i]->SetDisable();
	}
}

void AShinbi::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 늑대 배열 액터 삭제 및 초기화
	for (int i = 0; i < MaxWolfNum; ++i)
	{
		Wolves[i]->SetLifeSpan(0.01f);
		Wolves[i] = nullptr;
	}
	Wolves.Empty();
}
