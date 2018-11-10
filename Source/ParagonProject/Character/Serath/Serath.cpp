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

	// Heaven's Fury ĳ���� �� Ÿ�� ��Į ��ġ ����
	if (CurrentAbilityType == EAbilityType::AbilityQ)
	{
		FVector DecalLoc = GetMouseWorldPosition();
		DecalComp->SetWorldLocation(DecalLoc);
	}
	// Ascend ��ų�� ���� �ܰ��� �̵�
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
		// ���� ���콺 ��ġ�� ���̺��� ��ġ ����
		FlyTargetVec = AscendDecalLoc;
		CurrentAscendState = EAscendState::Dive;

		// Ascend Dive ��ƼŬ ����
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
	// ���������� Primary ���� �ִϸ��̼� ���
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
		// �߷� ���� �� ĳ���Ͱ� ���ư� ��ǥ ��ġ ����
		GetCharacterMovement()->GravityScale = 0.0f;
		FlyTargetVec = GetActorLocation() + GetActorUpVector() * 1500.0f;
		// ī�޶� ��ƼŬ ���� �� Ascend �ִϸ��̼� ���
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

		// ��ǥ ��ġ �̵� �� Fly ���·� ����
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

		// ��Į�� ���콺 ��ġ�� ����
		AscendDecalLoc = GetMouseWorldPosition();
		DecalComp->SetWorldLocation(AscendDecalLoc);
		break;

	case EAscendState::Dive:
		// Movment to target location
		MoveVec = FMath::VInterpTo(GetActorLocation(), FlyTargetVec, DeltaTime, AscendDiveSpeed);
		SetActorLocation(MoveVec);

		// ��ǥ ��ġ ������ ���� �� ���� �ʱ�ȭ
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
	// ī�޶� ��ġ�� ȸ������ ���� Ʈ���̼� �߻� ��ġ ����
	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector EndLoc = NewPos + NewDir * 10000.0f;

	// Ʈ���̽� �߻�
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

	// ���콺 ��ġ�� ���� ���������� ��ȯ
	CharCon->GetMousePosition(MouseX, MouseY);
	CharCon->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldPos, WorldDir);

	// ī�޶�� ���콺���� �Ÿ� ���ϱ�
	float Distance = GetTraceHitDistance(WorldPos, WorldDir);

	// ���� ���콺 ��ġ ��ȯ
	return WorldPos + WorldDir * Distance;
}