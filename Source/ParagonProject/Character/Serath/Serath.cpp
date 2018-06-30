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

	FlySpeed = 100.0f;

	// Primary 몽타주 애니메이션 섹션들
	PrimarySectionNames.Add(TEXT("A_Start"));
	PrimarySectionNames.Add(TEXT("B_Start"));
	PrimarySectionNames.Add(TEXT("C_Start"));
	PrimarySectionNames.Add(TEXT("D_Start"));
}

void ASerath::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	CharCon = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

void ASerath::BeginPlay()
{
	Super::BeginPlay();
}

void ASerath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFly)
	{
		FVector MoveVec = FMath::VInterpTo(GetActorLocation(), FlyTargetVec, DeltaTime, FlySpeed);
		SetActorRelativeLocation(MoveVec);
	}
	if (CurrentAbility == EAbilityType::AbilityQ)
	{
		FVector DecalLoc = GetMouseWorldPosition();
		DecalComp->SetWorldLocation(DecalLoc);
	}
}

void ASerath::StartPrimary()
{
	if (CurrentAbility == EAbilityType::AbilityQ)
	{
		SetHeavenFury();
	}
	else
	{
		Super::StartPrimary();
		CurrentAbility = EAbilityType::Primary;
	}
}

void ASerath::ComboAttack()
{
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

void ASerath::SetFly(bool bIsFly)
{
	bFly = bIsFly;
}

void ASerath::AbilityMR()
{
	Super::AbilityMR();

	PlayAnimMontage(WingFlapAnim);
	
}

void ASerath::AbilityQ()
{
	if (CurrentAbility != EAbilityType::AbilityQ)
	{
		DecalComp = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), Decal, FVector(140.0f, 140.0f, 140.0f), GetActorLocation());
		bAbilityFury = true;
		CurrentAbility = EAbilityType::AbilityQ;

		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		PlayAnimMontage(FuryAnim);
	}
	else if (CurrentAbility == EAbilityType::AbilityQ && bAbilityFury)
	{
		bAbilityFury = false;
		CurrentAbility = EAbilityType::None;
		PlayAnimMontage(FuryAnim, 1.0f, TEXT("Cancel"));
	}
}

void ASerath::AbilityE()
{
	Super::AbilityE();

	FlyTargetVec = GetActorLocation() + GetActorUpVector() * 1000.0f;
	CurrentAbility = EAbilityType::None;
	GetCharacterMovement()->GravityScale = 0.0f;
	PlayAnimMontage(FuryAnim, 1.0f, TEXT("Rise"));	
}

void ASerath::SetHeavenFury()
{
	DecalComp->SetLifeSpan(0.001f);

	PlayAnimMontage(FuryAnim, 1.0f, TEXT("Hover"));
	CurrentAbility = EAbilityType::None;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

float ASerath::GetTraceHitDistance(const FVector& NewPos, const FVector& NewDir)
{
	FVector CamLoc;
	FRotator CamRot;
	// 카메라 위치와 회전값을 통해 트레이서 발사 위치 구함
	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector EndLoc = NewPos + NewDir * 5000.0f;

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