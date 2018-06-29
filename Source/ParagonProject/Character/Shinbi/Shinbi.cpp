// Fill out your copyright notice in the Description page of Project Settings.

#include "Shinbi.h"
#include "ShinbiWolf.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AShinbi::AShinbi()
{
	PrimaryActorTick.bCanEverTick = false;

	/* 늑대 오브젝트 풀 */
	MaxWolfNum = 20;

	DashPower = 4500.0f;
	AttackWolvesDuration = 1.5f;

	/* Circling Wolves 스킬 */
	CircleWolfRate = 90.0f;
	CirclingWolvesDuration = 7.0f;

	/* Primary 스킬 */
	UltimateAngleInterval = 60.0f;
	UltimateSpawnTime = 0.5f;
	UltHitCount = 0;

	ObjPoolComp = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("Object Pool Component"));

}

void AShinbi::BeginPlay()
{
	Super::BeginPlay();

	// 늑대 액터 생성(오브젝트풀)
	ObjPoolComp->CreateObject(GetWorld(), WolfClass, GetActorLocation(), GetActorRotation(), MaxWolfNum);
}

void AShinbi::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("AbilityMR", IE_Released, this, &AShinbi::StopAttackCast);
}

void AShinbi::StartPrimary()
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
void AShinbi::AbilityMR()
{
	// Attack Wolves 스킬 애니메이션 몽타주 실행
	PlayAnimMontage(AttackWolvesMontage);
}

void AShinbi::StopAttackCast()
{
	MainUMG->GetAbilityBar()->SetAbility(AbilityComp->GetAbilityInfo(1));
	// 애님 몽타주의 End 섹션 재생
	PlayAnimMontage(AttackWolvesMontage, 1.0f, TEXT("End"));

	// 울프 액터 생성
	const FVector SpawnVec = GetActorLocation() + (GetActorForwardVector() * FVector(0, 0, -6.0f));
	AShinbiWolf* Wolf = ObjPoolComp->ActiveObject<AShinbiWolf>();
	if (Wolf)
	{
		Wolf->SetActorTransform(FTransform(GetActorRotation(), SpawnVec));
		Wolf->Action(EWolfState::Attack);
	}

	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &AShinbi::StopAttackWolves, Wolf);
	GetWorldTimerManager().SetTimer(CircleWolvesTimer, RespawnDelegate, AttackWolvesDuration, false);
}

void AShinbi::StopAttackWolves(AShinbiWolf* Wolf)
{
	Wolf->StopAttackWolves();
	ObjPoolComp->SetDisable(Wolf);
}

// Dash
void AShinbi::AbilityQ()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		Super::AbilityQ();

		PlayAnimMontage(DashMontage, 2.0f);
		SetCameraParticle(DashParticle); // 카메라에 대쉬 이펙트 실행

		// 정면으로 대쉬
		LaunchCharacter(GetActorForwardVector() * DashPower, true, true);
	}
}

// Circling Wolves 스킬
void AShinbi::AbilityE()
{
	if (bIsCircling)
	{
		return;
	}

	Super::AbilityE();

	// 플레이어 CircleWovles 애니메이션 실행
	PlayAnimMontage(CircleWolvesMontage);
	// 늑대들 생성
	float Angle = 0.0f;
	for (int i = 0; i < 4; ++i)
	{
		AShinbiWolf* Wolf = ObjPoolComp->ActiveObject<AShinbiWolf>();
		Wolf->SetCirclingAngle(Angle);
		Wolf->Action(EWolfState::Circle);
		CirclingWolfSet.Add(Wolf);

		Angle += CircleWolfRate;
	}
	// CirclingWolvesDuration 시간 후 울프 객체들 사라짐
	GetWorldTimerManager().SetTimer(CircleWolvesTimer, this, &AShinbi::StopCircleWolves, CirclingWolvesDuration, false);

	bIsCircling = true;
}

void AShinbi::StopCircleWolves()
{
	for (auto wolf : CirclingWolfSet)
	{
		// 제거 이펙트 생성 및 Disable
		wolf->StopCirclingWolves();
		wolf->Action(EWolfState::Idle);
		ObjPoolComp->SetDisable(wolf);
	}
	CirclingWolfSet.Empty();
	bIsCircling = false;
}

void AShinbi::Ultimate()
{
	TargetPawn = FocusView();
	if (TargetPawn != nullptr)
	{
		Super::Ultimate();

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
		TSet<AShinbiWolf*> UltWolfSet;
		for (int i = 0; i < 8; ++i)
		{
			AShinbiWolf* Wolf = ObjPoolComp->ActiveObject<AShinbiWolf>();
			// 늑대 위치, 상태 설정
			Wolf->Action(EWolfState::Ultimate);
			const FVector SpawnLoc = SetAngle(TargetPawn->GetActorLocation(), Angle);
			Wolf->SetupUltimate(TargetPawn, SpawnLoc, TimeRate);
			UltWolfSet.Add(Wolf);

			TimeRate += 0.15f;
			Angle += 15.0f;
		}
	}
	GetWorldTimerManager().SetTimer(MarkerTimer, this, &AShinbi::DestroyMarker, 0.6f, false);
}

// 타겟 위에 마커 이펙트 생성
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
		TargetPawn = nullptr;
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

void AShinbi::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 늑대 배열 액터 삭제 및 초기화
	//for (int i = 0; i < MaxWolfNum; ++i)
	//{
	//	Wolves[i]->SetLifeSpan(0.01f);
	//	Wolves[i] = nullptr;
	//}
	//Wolves.Empty();
}
