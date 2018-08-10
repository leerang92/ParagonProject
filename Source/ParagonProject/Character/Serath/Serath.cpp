// Fill out your copyright notice in the Description page of Project Settings.

#include "Serath.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "DrawDebugHelpers.h"

ASerath::ASerath()
{
	CurrentAbility = EAbilityType::None;
	AscendFlySpeed = 10.0f;
	AscendDiveSpeed = 20.0f;

	// Primary ��Ÿ�� �ִϸ��̼� ���ǵ�
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

	// Heaven's Fury ĳ���� �� Ÿ�� ��Į ��ġ ����
	if (CurrentAbility == EAbilityType::AbilityQ)
	{
		FVector DecalLoc = GetMouseWorldPosition();
		DecalComp->SetWorldLocation(DecalLoc);
	}
	// Ascend ��ų�� ���� �ܰ��� �̵�
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
		// ���� ���콺 ��ġ�� ���̺��� ��ġ ����
		FlyTargetVec = AscendDecalLoc;
		CurrentAscend = EAscendState::Dive;

		// Ascend Dive ��ƼŬ ����
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
	// ���������� Primary ���� �ִϸ��̼� ���
	if (SaveCombo && PrimaryAnim != nullptr)
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

void ASerath::SetFly()
{
	CurrentAscend = EAscendState::Intro;
}

// Wing Blast
void ASerath::AbilityMR()
{
	Super::AbilityMR();

	PlayAnimMontage(AbilityMRAnim);
	const FVector Origin = GetActorLocation() + GetActorForwardVector() * 200.0f;
	TArray<AActor*> IgnoreActor;
	bool bDamage = UGameplayStatics::ApplyRadialDamage(GetWorld(), 100.0f, Origin, 250.0f, UDamageType::StaticClass(), IgnoreActor, this);
	if (bDamage)
	{
		UE_LOG(LogClass, Warning, TEXT("Set Damage"));
	}

}

// Heaven's Fury
void ASerath::AbilityQ()
{
	if (CurrentAbility == EAbilityType::AbilityE)
	{
		return;
	}
	// Fury ��ų ����
	else if (CurrentAbility == EAbilityType::AbilityQ)
	{
		DecalComp->SetLifeSpan(0.01f);
		DecalComp = nullptr;
		PlayAnimMontage(AbilityQAnim, 1.0f, TEXT("Cancel"));
		CurrentAbility = EAbilityType::None;
	}
	// Fury ĳ��Ʈ ����
	else if (CurrentAbility != EAbilityType::AbilityQ)
	{
		Super::AbilityQ();
		SetMouseCenterLocation();
		DecalComp = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), Decal, FVector(140.0f, 140.0f, 140.0f), GetActorLocation());
		PlayAnimMontage(AbilityQAnim);

		CurrentAbility = EAbilityType::AbilityQ;
	}
}

// Acend
void ASerath::AbilityE()
{
	if (CurrentAbility == EAbilityType::AbilityE || CurrentAbility == EAbilityType::AbilityQ)
	{
		return;
	}
	Super::AbilityE();

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	// �߷� ���� �� ĳ���Ͱ� ���ư� ��ǥ ��ġ ����
	GetCharacterMovement()->GravityScale = 0.0f;
	FlyTargetVec = GetActorLocation() + GetActorUpVector() * 1500.0f;
	// ī�޶� ��ƼŬ ���� �� Ascend �ִϸ��̼� ���
	SetCameraParticle(AscendCamFX);
	PlayAnimMontage(AbilityEAnim);
	CurrentAbility = EAbilityType::AbilityE;
	
}

void ASerath::SetHeavenFury()
{
	// ��Į ����
	DecalComp->SetLifeSpan(0.001f);

	PlayAnimMontage(AbilityQAnim, 1.0f, TEXT("Hover"));
	//GetCharacterMovement()->bUseControllerDesiredRotation = true;

	CurrentAbility = EAbilityType::None;
}

void ASerath::MovementAscendAbility(const float DeltaTime)
{
	FVector MoveVec, DistVec;
	switch (CurrentAscend)
	{
	case EAscendState::Intro: // ����
		// ���߿� ��ǥ ��ġ���� �̵�
		MoveVec = FMath::VInterpTo(GetActorLocation(), FlyTargetVec, DeltaTime, AscendFlySpeed);
		SetActorLocation(MoveVec);

		// ��ǥ ��ġ �̵� �� Fly ���·� ����
		DistVec = FlyTargetVec - GetActorLocation();
		if (DistVec.Z < 0.5f)
		{
			CurrentAscend = EAscendState::Fly;
		}
		break;

	case EAscendState::Fly: // ���߿� ���� ��
		if (DecalComp == nullptr)
		{
			SetMouseCenterLocation();
			// Ÿ�� ��Į ����
			DecalComp = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), Decal, FVector(240.0f, 240.0f, 240.0f), GetActorLocation());
		}

		// ��Į�� ���콺 ��ġ�� ����
		AscendDecalLoc = GetMouseWorldPosition();
		DecalComp->SetWorldLocation(AscendDecalLoc);
		break;

	case EAscendState::Dive: // ���̺�
		// ��ǥ�� �̵�
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

			// ���� �ʱ�ȭ
			CurrentAscend = EAscendState::None;
			CurrentAbility = EAbilityType::None;
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
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