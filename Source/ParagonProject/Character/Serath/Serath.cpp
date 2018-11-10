// Fill out your copyright notice in the Description page of Project Settings.

#include "Serath.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"

ASerath::ASerath()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentAbilityType = EAbilityType::None;
	AscendFlySpeed = 10.0f;
	AscendDiveSpeed = 20.0f;

	// Register primary montage animation section name
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

	//UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	//if (AnimInst != nullptr)
	//{
	//	AnimInst->AddCurveValue(FName("Holy=True"), 1.0f);
	//}
}

void ASerath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Heaven's Fury 캐스팅 중 타겟 데칼 위치 설정
	if (CurrentAbilityType == EAbilityType::AbilityQ)
	{
		FVector DecalLoc = GetMouseWorldPosition();
		DecalComp->SetWorldLocation(DecalLoc);
	}
	// Ascend 스킬에 따른 단계적 이동
	if (CurrentAscendState != EAscendState::None)
	{
		MovementAscendAbility(DeltaTime);
	}
}

void ASerath::StartPrimary()
{
	if (CurrentAbilityType == EAbilityType::AbilityQ)
	{
		OnHeavenFuryHover();
	}
	else if (CurrentAbilityType == EAbilityType::AbilityE
		&& CurrentAscendState == EAscendState::Fly)
	{
		// 현재 마우스 위치로 다이브할 위치 설정
		FlyTargetVec = AscendDecalLoc;
		CurrentAscendState = EAscendState::Dive;

		// Ascend Dive 파티클 생성
		AscendFXComp = UGameplayStatics::SpawnEmitterAttached(AscendDiveFX, GetMesh(), TEXT("Root"));
	}
	else
	{
		Super::StartPrimary();
		CurrentAbilityType = EAbilityType::Primary;
	}
}

void ASerath::ComboAttack()
{
	// 순차적으로 Primary 공격 애니메이션 재생
	if (bSaveCombo && PrimaryAnim != nullptr)
	{
		Super::ComboAttack();

		PlayAnimMontage(PrimaryAnim, 1.5f, PrimarySectionNames[SectionCount++]);

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

void ASerath::OnParticleToHitActor()
{
	if (HitActors.Num() > 0 && FuryImpactFX != nullptr)
	{
		for (auto& actor : HitActors)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FuryImpactFX, actor->GetActorLocation());
		}
	}

	HitActors.Reset();
}

void ASerath::SetFly()
{
	CurrentAscendState = EAscendState::Intro;
}

// Wing Blast
void ASerath::AbilityMR()
{
	FAbilityInfo* Ability = AbilityComp->GetAbilityInfoPointer(1);
	if (Ability->bUsable)
	{
		Ability->bUsable = false;

		Super::AbilityMR();

		PlayAnimMontage(AbilityMRAnim);

		FVector Origin = GetActorLocation() + GetActorForwardVector() * 200.0f;
		OnRangeDamage(Origin, 100.0f, 250.0f);
	}
}

// Heaven's Fury
void ASerath::AbilityQ()
{
	if (CurrentAbilityType == EAbilityType::AbilityE)
	{
		return;
	}

	bool bUsable = AbilityComp->GetAbilityInfo((int)EAbilityType::AbilityQ).bUsable;
	if (bUsable)
	{
		// Cancel
		if (CurrentAbilityType == EAbilityType::AbilityQ)
		{
			DecalComp->SetLifeSpan(0.01f);
			DecalComp = nullptr;
			PlayAnimMontage(AbilityQAnim, 1.0f, TEXT("Cancel"));
			CurrentAbilityType = EAbilityType::None;
		}
		// Start casting
		else if (CurrentAbilityType != EAbilityType::AbilityQ)
		{
			SetMouseCenterLocation();
			DecalComp = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), Decal, FVector(140.0f, 140.0f, 140.0f), GetActorLocation());
			PlayAnimMontage(AbilityQAnim);

			CurrentAbilityType = EAbilityType::AbilityQ;
		}
	}
}

// Acend
void ASerath::AbilityE()
{
	if (CurrentAbilityType == EAbilityType::AbilityE
		|| CurrentAbilityType == EAbilityType::AbilityQ)
	{
		return;
	}

	bool bUsable = AbilityComp->GetAbilityInfo((int)EAbilityType::AbilityE).bUsable;

	if (bUsable)
	{
		AbilityComp->GetAbilityInfoPointer((int)EAbilityType::AbilityE)->bUsable = false;

		Super::AbilityE();

		bMovement = false;
		// 중력 제거 및 캐릭터가 날아갈 목표 위치 지정
		GetCharacterMovement()->GravityScale = 0.0f;
		FlyTargetVec = GetActorLocation() + GetActorUpVector() * 1500.0f;
		// 카메라 파티클 생성 및 Ascend 애니메이션 재생
		SetCameraParticle(AscendCamFX);
		PlayAnimMontage(AbilityEAnim);

		CurrentAbilityType = EAbilityType::AbilityE;
	}
}

void ASerath::Ultimate()
{
	Super::Ultimate();

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst != nullptr)
	{
		float holy = 0.0f;
		AnimInst->AddCurveValue(FName("Holy=True"), holy);
		//AnimInst->GetCurveValue(FName("Holy=True"), holy);
		UE_LOG(LogClass, Warning, TEXT("Set Curve : %f"), AnimInst->GetCurveValue(FName("Holy=True")));
		/*bool Curve = AnimInst->GetCurveValue(FName("Holy=True"), holy);
		if (Curve)
		{
			UE_LOG(LogClass, Warning, TEXT("Set Curve : %f"), AnimInst->GetCurveValue(FName("Holy=True")));

		}*/
	}
}

void ASerath::OnHeavenFuryHover()
{
	FAbilityInfo* Ability = AbilityComp->GetAbilityInfoPointer((int)EAbilityType::AbilityQ);

	if (Ability->bUsable)
	{
		// Set Ability Icon And CoolDown
		Ability->bUsable = false;
		SetAbilityHUD((int)EAbilityType::AbilityQ);

		DecalComp->SetLifeSpan(0.001f);

		bMovement = false;
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		PlayAnimMontage(AbilityQAnim, 1.0f, TEXT("Hover"));
		float AnimPlayDuration = AbilityQAnim->GetSectionLength(3);
		GetWorldTimerManager().SetTimer(WaitTimer, this, &ASerath::StopHeavenFury, AnimPlayDuration, false);
	}
}

void ASerath::StopHeavenFury()
{
	bMovement = true;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	CurrentAbilityType = EAbilityType::None;
	GetWorldTimerManager().ClearTimer(WaitTimer);
}


void ASerath::OnFuryDamage()
{
	// Set Damage
	FVector Origin = DecalComp->RelativeLocation;
	OnRangeDamage(Origin, 100.0f, 250.0f);

	OnParticleToHitActor();
}

void ASerath::MovementAscendAbility(const float DeltaTime)
{
	FVector MoveVec, DistVec;
	switch (CurrentAscendState)
	{
	case EAscendState::Intro:
		// Fly to target location
		MoveVec = FMath::VInterpTo(GetActorLocation(), FlyTargetVec, DeltaTime, AscendFlySpeed);
		SetActorLocation(MoveVec);

		// 목표 위치 이동 후 Fly 상태로 변경
		DistVec = FlyTargetVec - GetActorLocation();
		if (DistVec.Z < 0.5f)
		{
			CurrentAscendState = EAscendState::Fly;
		}
		break;

	case EAscendState::Fly:
		if (DecalComp == nullptr)
		{
			SetMouseCenterLocation();
			// Spawn Target Decal
			DecalComp = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), Decal, FVector(240.0f, 240.0f, 240.0f), GetActorLocation());
		}

		// 데칼을 마우스 위치에 생성
		AscendDecalLoc = GetMouseWorldPosition();
		DecalComp->SetWorldLocation(AscendDecalLoc);
		break;

	case EAscendState::Dive:
		// Movment to target location
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
			PlayAnimMontage(AbilityEAnim, 1.0f, TEXT("Land"));

			// Reset state
			CurrentAscendState = EAscendState::None;
			CurrentAbilityType = EAbilityType::None;
			bMovement = true;
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