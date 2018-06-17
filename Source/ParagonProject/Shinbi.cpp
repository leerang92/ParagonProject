// Fill out your copyright notice in the Description page of Project Settings.

#include "Shinbi.h"
#include "ShinbiWolf.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Camera/CameraComponent.h"

AShinbi::AShinbi()
{
	DashPower = 4500.0f;
	WolfInterval = 90.0f;
	WolfIndex = 0;
	CirclingWolvesDuration = 7.0f;

	WolfIndex = 0;
	MaxWolfNum = 15;

	CountDown = 5.0f;
	PrimaryInterval = CountDown;
	PrimaryAngle = 0.0f;
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

void AShinbi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPrimary)
	{
		CountDown -= DeltaTime;
		if (CountDown < 0.0f)
		{
			bIsPrimary = false;
		}
		else if (CountDown <= PrimaryInterval)
		{
			SetPrimaryWolves(TargetLoc, PrimaryAngle);
			PrimaryInterval -= 1.0f;
			PrimaryAngle += 45.0f;
		}
	}
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

void AShinbi::StopAttack()
{
	SaveCombo = false;
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

void AShinbi::ResetComboAttack()
{
	Super::ResetComboAttack();
}

// Attack Wolves 스킬
void AShinbi::AbilityMouseR()
{
	// Attack Wolves 스킬 애니메이션 몽타주 실행
	PlayAnimMontage(AttackWolvesMontage);
	IsAttacking = true;
}

void AShinbi::StopAttackCast()
{
	// 애님 몽타주의 End 섹션 재생
	PlayAnimMontage(AttackWolvesMontage, 1.0f, TEXT("End"));

	// 울프 액터 생성
	const FVector SpawnVec = GetActorLocation() + (GetActorForwardVector() * FVector(0, 0, -6.0f));
	SetupWolves(SpawnVec, GetActorRotation(), 1, 1);
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
	CirclingIndexes = SetupWolves(GetActorLocation(), GetActorRotation(), 2, 4);
	// 늑대들 각도 설정
	float Angle = 0.0f;
	for (auto index : CirclingIndexes)
	{
		Wolves[index]->SetCirclingAngle(Angle);
		Angle += WolfInterval;
	}
	// CirclingWolvesDuration 시간 후 울프 객체들 사라짐
	GetWorldTimerManager().SetTimer(CircleWolvesTimer, this, &AShinbi::StopCircleWolves, CirclingWolvesDuration, false);

	bIsCircling = true;
}

void AShinbi::StopCircleWolves()
{
	for (auto Index : CirclingIndexes)
	{
		// 제거 이펙트 생성 및 Disable
		Wolves[Index]->StopCirclingWolves();
	}
	CirclingIndexes.Empty();
	bIsCircling = false;
}

void AShinbi::PrimaryAbility()
{
	const APawn* Target = FocusView();
	if (Target != nullptr)
	{
		TargetLoc = Target->GetActorLocation() + FVector(0, 0, 100.0f);
		bIsPrimary = true;
	}
}

void AShinbi::SetPrimaryWolves(FVector NewLocation, float NewAngle)
{
	const FVector RotationVec = FVector(80.0f, 0, 0);
	const FVector Axis = FVector(0, 0, 1).GetSafeNormal();
	const FVector ResultVec = RotationVec.RotateAngleAxis(PrimaryAngle, Axis);
	NewLocation += ResultVec;

	TSet<int> IndexSet = SetupWolves(NewLocation, FRotator::ZeroRotator, 3, 1);



	/*FSetElementId SetId = IndexSet.FindId(0);
	int index = IndexSet[SetId];
	Wolves[index]->SetActorLocation(NewLocation);*/
}

TSet<int> AShinbi::SetupWolves(const FVector SpawnVec, const FRotator SpawnRot, uint8 Type, int SpawnNum)
{
	TSet<int> Indexes;
	// 현재 사용중이지 않은 늑대 액터 설정
	for (int SpawnCount = 0; SpawnCount < SpawnNum;)
	{
		if (!Wolves[WolfIndex]->IsHiddenEd())
		{
			Wolves[WolfIndex]->SetEnable();
			Wolves[WolfIndex]->SetActorTransform(FTransform(SpawnRot, SpawnVec));
			Wolves[WolfIndex]->Action(static_cast<EWolfState>(Type));
			Indexes.Add(WolfIndex);
			++SpawnCount;
		}
		// 인덱스 순환
		++WolfIndex %= MaxWolfNum;
	}
	return Indexes;
}

void AShinbi::CreateWolves()
{
	for (int i = 0; i < MaxWolfNum; ++i)
	{
		Wolves.Add(GetWorld()->SpawnActor<AShinbiWolf>(WolfClass, GetActorLocation(), GetActorRotation()));
		Wolves[i]->SetDisable();
	}
}

void AShinbi::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (int i = 0; i < MaxWolfNum; ++i)
	{
		Wolves[i]->SetLifeSpan(0.01f);
		Wolves[i] = nullptr;
	}
	Wolves.Empty();
}
