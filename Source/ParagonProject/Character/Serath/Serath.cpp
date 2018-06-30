// Fill out your copyright notice in the Description page of Project Settings.

#include "Serath.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

ASerath::ASerath()
{
	CurrentAbility = EAbilityType::None;

	SectionCount = 0;

	AscendFlySpeed = 10.0f;
	AscendDiveSpeed = 20.0f;

	// Primary 몽타주 애니메이션 섹션들
	PrimarySectionNames.Add(TEXT("A_Start"));
	PrimarySectionNames.Add(TEXT("B_Start"));
	PrimarySectionNames.Add(TEXT("C_Start"));
	PrimarySectionNames.Add(TEXT("D_Start"));
}

void ASerath::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASerath::BeginPlay()
{
	Super::BeginPlay();

	CharCon = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

void ASerath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Heaven's Fury 캐스팅 중 타겟 데칼 위치 설정
	if (CurrentAbility == EAbilityType::AbilityQ)
	{
		FVector DecalLoc = GetMouseWorldPosition();
		DecalComp->SetWorldLocation(DecalLoc);
	}
	// Ascend 스킬에 따른 단계적 이동
	if (CurrentAscend != EAscendState::None)
	{
		MovementAscendAbility(DeltaTime);
	}
}

void ASerath::StartPrimary()
{
	if (CurrentAbility == EAbilityType::AbilityQ)
	{
		SetHeavenFury();
	}
	else if (CurrentAscend == EAscendState::Fly)
	{
		// 현재 마우스 위치로 다이브할 위치 설정
		FlyTargetVec = AscendDecalLoc;
		CurrentAscend = EAscendState::Dive;

		// Ascend Dive 파티클 생성
		AscendFXComp = UGameplayStatics::SpawnEmitterAttached(AscendDiveFX, GetMesh(), TEXT("Root"));
	}
	else
	{
		Super::StartPrimary();
		CurrentAbility = EAbilityType::Primary;
	}
}

void ASerath::ComboAttack()
{
	// 순차적으로 Primary 공격 애니메이션 재생
	if (SaveCombo && AttackMontages.Num() > 0)
	{
		PlayAnimMontage(AttackMontages[0], 1.5f, PrimarySectionNames[SectionCount++]);

		if (PrimarySectionNames.Num() <= SectionCount)
		{
			SectionCount = 0;
		}
	}
}

void ASerath::ResetComboAttack()
{
	Super::ResetComboAttack();
	SectionCount = 0;
}

void ASerath::SetFly()
{
	CurrentAscend = EAscendState::Intro;
}

// Wing Blast
void ASerath::AbilityMR()
{
	Super::AbilityMR();

	PlayAnimMontage(WingFlapAnim);
}

// Heaven's Fury
void ASerath::AbilityQ()
{
	// Fury 스킬 중지
	if (CurrentAbility == EAbilityType::AbilityQ)
	{
		DecalComp->SetLifeSpan(0.01f);
		DecalComp = nullptr;
		PlayAnimMontage(FuryAnim, 1.0f, TEXT("Cancel"));
		CurrentAbility = EAbilityType::None;
	}
	// Fury 캐스트 시작
	else if (CurrentAbility != EAbilityType::AbilityQ)
	{
		DecalComp = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), Decal, FVector(140.0f, 140.0f, 140.0f), GetActorLocation());
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		PlayAnimMontage(FuryAnim);

		CurrentAbility = EAbilityType::AbilityQ;
	}
}

// Acend
void ASerath::AbilityE()
{
	Super::AbilityE();

	// 중력 제거 및 캐릭터가 날아갈 목표 위치 지정
	GetCharacterMovement()->GravityScale = 0.0f;
	FlyTargetVec = GetActorLocation() + GetActorUpVector() * 1500.0f;

	// 카메라 파티클 생성 및 Ascend 애니메이션 재생
	SetCameraParticle(AscendCamFX);
	PlayAnimMontage(AscendAnim);

	CurrentAbility = EAbilityType::AbilityE;
}

void ASerath::SetHeavenFury()
{
	// 데칼 삭제
	DecalComp->SetLifeSpan(0.001f);

	PlayAnimMontage(FuryAnim, 1.0f, TEXT("Hover"));
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	CurrentAbility = EAbilityType::None;
}

void ASerath::MovementAscendAbility(const float DeltaTime)
{
	FVector MoveVec, DistVec;
	switch (CurrentAscend)
	{
	case EAscendState::Intro: // 시작
		// 공중에 목표 위치까지 이동
		MoveVec = FMath::VInterpTo(GetActorLocation(), FlyTargetVec, DeltaTime, AscendFlySpeed);
		SetActorLocation(MoveVec);

		// 목표 위치 이동 후 Fly 상태로 변경
		DistVec = FlyTargetVec - GetActorLocation();
		if (DistVec.Z < 0.5f)
		{
			CurrentAscend = EAscendState::Fly;
		}
		break;

	case EAscendState::Fly: // 공중에 있을 때
		if (DecalComp == nullptr)
		{
			// 타겟 데칼 생성
			DecalComp = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), Decal, FVector(240.0f, 240.0f, 240.0f), GetActorLocation());
		}

		// 데칼을 마우스 위치에 생성
		AscendDecalLoc = GetMouseWorldPosition();
		DecalComp->SetWorldLocation(AscendDecalLoc);
		break;

	case EAscendState::Dive: // 다이브
		// 목표로 이동
		MoveVec = FMath::VInterpTo(GetActorLocation(), FlyTargetVec, DeltaTime, AscendDiveSpeed);
		SetActorLocation(MoveVec);

		// 목표 위치 도착후 착지 및 변수 초기화
		DistVec = FlyTargetVec - GetActorLocation();
		if (DistVec.Z > -30.0f)
		{
			AscendFXComp->Deactivate();
			DecalComp->SetLifeSpan(0.01f);
			DecalComp = nullptr;

			GetCharacterMovement()->GravityScale = 1.0f;
			PlayAnimMontage(AscendAnim, 1.0f, TEXT("Land"));

			// 상태 초기화
			CurrentAscend = EAscendState::None;
			CurrentAbility = EAbilityType::None;
		}
		break;
	}
}

float ASerath::GetTraceHitDistance(const FVector& NewPos, const FVector& NewDir)
{
	FVector CamLoc;
	FRotator CamRot;
	// 카메라 위치와 회전값을 통해 트레이서 발사 위치 구함
	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector EndLoc = NewPos + NewDir * 10000.0f;

	//DrawDebugLine(GetWorld(), CamLoc, EndLoc, FColor::Red, false, 3.0f, 3.0f);
	// 트레이스 발사
	FCollisionQueryParams TraceParam(TEXT("DecalTrace"), true, this);
	TraceParam.bTraceAsyncScene = true;
	TraceParam.bReturnPhysicalMaterial = false;
	TraceParam.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, EndLoc, ECC_WorldStatic, TraceParam);

	return Hit.Distance;
}

FVector ASerath::GetMouseWorldPosition()
{
	float MouseX, MouseY;
	FVector WorldPos, WorldDir;

	// 마우스 위치를 월드 포지션으로 변환
	CharCon->GetMousePosition(MouseX, MouseY);
	CharCon->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldPos, WorldDir);

	// 카메라와 마우스간의 거리 구하기
	float Distance = GetTraceHitDistance(WorldPos, WorldDir);

	// 최종 마우스 위치 반환
	return WorldPos + WorldDir * Distance;
}