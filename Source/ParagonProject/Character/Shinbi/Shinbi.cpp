// Fill out your copyright notice in the Description page of Project Settings.

#include "Shinbi.h"
#include "ShinbiWolf.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AShinbi::AShinbi()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentAbility = EAbilityType::None;

	/* ���� ������Ʈ Ǯ */
	MaxWolfNum = 20;

	DashPower = 4500.0f;
	AttackWolvesDuration = 1.5f;

	/* Circling Wolves ��ų */
	CircleWolfRate = 90.0f;
	CirclingWolvesDuration = 7.0f;

	/* Primary ��ų */
	UltimateAngleInterval = 60.0f;
	UltimateSpawnTime = 0.5f;
	UltHitCount = 0;

	PrimarySectionNames.Add(TEXT("Primary_B"));
	PrimarySectionNames.Add(TEXT("Primary_C"));
	PrimarySectionNames.Add(TEXT("Primary_D"));

	ObjPoolComp = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("Object Pool Component"));
}

void AShinbi::BeginPlay()
{
	Super::BeginPlay();

	// ���� ���� ����(������ƮǮ)
	ObjPoolComp->CreateObject(GetWorld(), WolfClass, GetActorLocation(), GetActorRotation(), MaxWolfNum);
}

void AShinbi::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("AbilityMR", IE_Released, this, &AShinbi::StopAttackCast);
}

//void AShinbi::StartPrimary()
//{
//	// �������� �ƴ� ��
//	if (CurrentAbility == EAbilityType::None)
//	{
//		SaveCombo = true;
//		ComboAttack();
//	}
//	// �̹� �������� �� �޺� �̾��ϱ�
//	else if (!SaveCombo)
//	{
//		SaveCombo = true;
//	}
//}

void AShinbi::ComboAttack()
{
	if (SaveCombo && AttackMontages.Num() > 0)
	{
		// ���� ��Ÿ�� �ִϸ��̼� ���
		PlayAnimMontage(PrimaryAnim, 1.0f, PrimarySectionNames[SectionCount++]);
		CurrentAbility = EAbilityType::Primary;

		// ������ �����̸� ���� ī��Ʈ �ʱ�ȭ
		if (AttackMontages.Num() <= SectionCount)
		{
			SectionCount = 0;
		}
	}
}

// Attack Wolves ��ų
void AShinbi::AbilityMR()
{
	// Attack Wolves ��ų �ִϸ��̼� ��Ÿ�� ����
	PlayAnimMontage(AbilityMRAnim);
	CurrentAbility = EAbilityType::MouseR;
}

void AShinbi::StopAttackCast()
{
	MainUMG->GetAbilityBar()->SetAbilityUI(AbilityComp->GetAbilityInfo(1));
	// �ִ� ��Ÿ���� End ���� ���
	PlayAnimMontage(AbilityMRAnim, 1.0f, TEXT("End"));

	// ���� ���� ����
	const FVector SpawnVec = GetActorLocation() + (GetActorForwardVector() * FVector(0, 0, -6.0f));
	AShinbiWolf* Wolf = ObjPoolComp->ActiveObject<AShinbiWolf>();
	if (Wolf)
	{
		Wolf->SetActorTransform(FTransform(GetActorRotation(), SpawnVec));
		Wolf->Action(EWolfState::Attack);
	}

	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &AShinbi::StopAttackWolves, Wolf);
	GetWorldTimerManager().SetTimer(CircleWolvesTimer, RespawnDelegate, AttackWolvesDuration, false);
	CurrentAbility = EAbilityType::None;
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

		PlayAnimMontage(AbilityQAnim, 2.0f);
		SetCameraParticle(DashParticle); // ī�޶� �뽬 ����Ʈ ����

		// �������� �뽬
		LaunchCharacter(GetActorForwardVector() * DashPower, true, true);
		CurrentAbility = EAbilityType::AbilityQ;
	}
}

// Circling Wolves ��ų
void AShinbi::AbilityE()
{
	if (!bCircling)
	{
		Super::AbilityE();

		// �÷��̾� CircleWovles �ִϸ��̼� ����
		PlayAnimMontage(AbilityEAnim);
		// ����� ����
		float Angle = 0.0f;
		for (int i = 0; i < 4; ++i)
		{
			AShinbiWolf* Wolf = ObjPoolComp->ActiveObject<AShinbiWolf>();
			Wolf->SetCirclingAngle(Angle);
			Wolf->Action(EWolfState::Circle);
			CirclingWolfSet.Add(Wolf);

			Angle += CircleWolfRate;
		}
		// CirclingWolvesDuration �ð� �� ���� ��ü�� �����
		GetWorldTimerManager().SetTimer(CircleWolvesTimer, this, &AShinbi::StopCircleWolves, CirclingWolvesDuration, false);
		CurrentAbility = EAbilityType::AbilityE;
		bCircling = true;
	}
}

void AShinbi::StopCircleWolves()
{
	for (auto wolf : CirclingWolfSet)
	{
		// ���� ����Ʈ ���� �� Disable
		wolf->StopCirclingWolves();
		wolf->Action(EWolfState::Idle);
		ObjPoolComp->SetDisable(wolf);
	}
	CirclingWolfSet.Empty();
	bCircling = false;
	CurrentAbility = EAbilityType::None;
}

void AShinbi::Ultimate()
{
	if (CurrentAbility == EAbilityType::Ultimate)
	{
		return;
	}

	TargetPawn = FocusView();
	if (TargetPawn != nullptr)
	{
		Super::Ultimate();

		// Ÿ�ٿ��� ��Ŀ ����Ʈ ����
		MarkerFX = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), UltMarker, TargetPawn->GetActorTransform());

		// �ִϸ��̼� ����
		PlayAnimMontage(UltimateAnim);

		// Ÿ�ٰ��� ���� ����
		FVector Dir = TargetPawn->GetActorLocation() - GetActorLocation();
		float Angle = FMath::Atan2(Dir.Z, Dir.X);
		Angle = FMath::RadiansToDegrees(Angle);
		Angle += 130.0f;

		float TimeRate = 0.6f;
		// ���� ����
		TSet<AShinbiWolf*> UltWolfSet;
		for (int i = 0; i < 8; ++i)
		{
			AShinbiWolf* Wolf = ObjPoolComp->ActiveObject<AShinbiWolf>();
			// ���� ��ġ, ���� ����
			Wolf->Action(EWolfState::Ultimate);
			const FVector SpawnLoc = SetAngle(TargetPawn->GetActorLocation(), Angle);
			Wolf->SetupUltimate(TargetPawn, SpawnLoc, TimeRate);
			UltWolfSet.Add(Wolf);

			TimeRate += 0.15f;
			Angle += 15.0f;
		}
	}
	GetWorldTimerManager().SetTimer(MarkerTimer, this, &AShinbi::DestroyMarker, 0.6f, false);
	CurrentAbility = EAbilityType::Ultimate;
}

// Ÿ�� ���� ��Ŀ ����Ʈ ����
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
		CurrentAbility = EAbilityType::None;
	}
}

void AShinbi::CameraShake()
{
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), CamShake, GetActorLocation(), 0.0f, 500.0f);
}

FVector AShinbi::SetAngle(FVector NewLocation, float NewAngle)
{
	/* Ÿ���� �߽����� PrimaryAngle��ŭ ���� ��ġ�� ���� */
	const FVector RotationVec = FVector(400.0f, 0, 140.0f);
	const FVector Axis = FVector(0, 0, 1).GetSafeNormal();
	const FVector ResultVec = RotationVec.RotateAngleAxis(NewAngle, Axis);
	NewLocation += ResultVec;

	return NewLocation;
}

void AShinbi::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
