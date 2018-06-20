// Fill out your copyright notice in the Description page of Project Settings.

#include "Shinbi.h"
#include "ShinbiWolf.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AShinbi::AShinbi()
{
	PrimaryActorTick.bCanEverTick = false;

	/* ���� ������Ʈ Ǯ */
	MaxWolfNum = 20;
	WolfIndex = 0;

	DashPower = 4500.0f;

	/* Circling Wolves ��ų */
	CircleWolfRate = 90.0f;
	CirclingWolvesDuration = 7.0f;

	/* Primary ��ų */
	UltimateAngleInterval = 60.0f;
	UltimateSpawnTime = 0.5f;
	UltHitCount = 0;
}

void AShinbi::BeginPlay()
{
	Super::BeginPlay();

	// ���� ���� ����(������ƮǮ)
	CreateWolves();
}

void AShinbi::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("AbilityMouseR", IE_Released, this, &AShinbi::StopAttackCast);
}

void AShinbi::StartAttack()
{
	// �������� �ƴ� ��
	if (!IsAttacking)
	{
		IsAttacking = true;
		SaveCombo = true;
		ComboAttack();
	}
	// �̹� �������� �� �޺� �̾��ϱ�
	else if (!SaveCombo)
	{
		SaveCombo = true;
	}
}

void AShinbi::ComboAttack()
{
	if (SaveCombo && AttackMontages.Num() > 0)
	{
		// ���� ��Ÿ�� �ִϸ��̼� ���
		PlayAnimMontage(AttackMontages[AttackCount++]);

		// ������ �����̸� ���� ī��Ʈ �ʱ�ȭ
		if (AttackMontages.Num() <= AttackCount)
		{
			AttackCount = 0;
		}
	}
}

// Attack Wolves ��ų
void AShinbi::AbilityMouseR()
{
	// Attack Wolves ��ų �ִϸ��̼� ��Ÿ�� ����
	PlayAnimMontage(AttackWolvesMontage);
}

void AShinbi::StopAttackCast()
{
	// �ִ� ��Ÿ���� End ���� ���
	PlayAnimMontage(AttackWolvesMontage, 1.0f, TEXT("End"));

	// ���� ���� ����
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
		SetCameraParticle(DashParticle); // ī�޶� �뽬 ����Ʈ ����

		// �������� �뽬
		LaunchCharacter(GetActorForwardVector() * DashPower, true, true);
	}
}

// Circling Wolves ��ų
void AShinbi::Ability2()
{
	if (bIsCircling)
	{
		return;
	}

	// �÷��̾� CircleWovles �ִϸ��̼� ����
	PlayAnimMontage(CircleWolvesMontage);
	// ����� ����
	CirclingWolfSet = ActiveWolves(GetActorLocation(), GetActorRotation(), 4);
	// ����� ���� ����
	float Angle = 0.0f;
	for (auto wolf : CirclingWolfSet)
	{
		wolf->SetCirclingAngle(Angle);
		wolf->Action(EWolfState::Circle);
		Angle += CircleWolfRate;
	}
	// CirclingWolvesDuration �ð� �� ���� ��ü�� �����
	//GetWorldTimerManager().SetTimer(CircleWolvesTimer, this, &AShinbi::StopCircleWolves, CirclingWolvesDuration, false);

	bIsCircling = true;
}

void AShinbi::StopCircleWolves()
{
	for (auto wolf : CirclingWolfSet)
	{
		// ���� ����Ʈ ���� �� Disable
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
		// Ÿ�ٿ��� ��Ŀ ����Ʈ ����
		MarkerFX = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), UltMarker, TargetPawn->GetActorTransform());

		// �ִϸ��̼� ����
		PlayAnimMontage(UltMontage);

		// Ÿ�ٰ��� ���� ����
		FVector Dir = TargetPawn->GetActorLocation() - GetActorLocation();
		float Angle = FMath::Atan2(Dir.Z, Dir.X);
		Angle = FMath::RadiansToDegrees(Angle);
		Angle += 130.0f;

		float TimeRate = 0.6f;
		// ���� ����
		TSet<AShinbiWolf*> WolfSet = ActiveWolves(FVector::ZeroVector, FRotator::ZeroRotator, 8);
		for (auto& wolf : WolfSet)
		{
			// ���� ��ġ, ���� ����
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
	/* Ÿ���� �߽����� PrimaryAngle��ŭ ���� ��ġ�� ���� */
	const FVector RotationVec = FVector(400.0f, 0, 140.0f);
	const FVector Axis = FVector(0, 0, 1).GetSafeNormal();
	const FVector ResultVec = RotationVec.RotateAngleAxis(NewAngle, Axis);
	NewLocation += ResultVec;

	return NewLocation;
}

TSet<AShinbiWolf*> AShinbi::ActiveWolves(const FVector SpawnVec, const FRotator SpawnRot, int SpawnNum)
{
	TSet<AShinbiWolf*> WolfSet;
	// ���� ��������� ���� ���� ���� ����
	for (int SpawnCount = 0; SpawnCount < SpawnNum;)
	{
		if (!Wolves[WolfIndex]->IsHiddenEd())
		{
			Wolves[WolfIndex]->SetEnable();
			Wolves[WolfIndex]->SetActorTransform(FTransform(SpawnRot, SpawnVec));
			WolfSet.Add(Wolves[WolfIndex]);
			++SpawnCount;
		}
		// �ε��� ��ȯ
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

	// ���� �迭 ���� ���� �� �ʱ�ȭ
	for (int i = 0; i < MaxWolfNum; ++i)
	{
		Wolves[i]->SetLifeSpan(0.01f);
		Wolves[i] = nullptr;
	}
	Wolves.Empty();
}
