// Fill out your copyright notice in the Description page of Project Settings.

#include "Shinbi.h"
#include "ShinbiWolf.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Camera/CameraComponent.h"

AShinbi::AShinbi()
{
	PrimaryActorTick.bCanEverTick = false;

	/* ���� ������Ʈ Ǯ */
	MaxWolfNum = 15;
	WolfIndex = 0;

	DashPower = 4500.0f;

	/* Circling Wolves ��ų */
	WolfInterval = 90.0f;
	CirclingWolvesDuration = 7.0f;

	/* Primary ��ų */
	PrimaryAngleInterval = 60.0f;
	PrimarySpawnTime = 0.5f;
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

void AShinbi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	SetupWolves(SpawnVec, GetActorRotation(), 1, 1);
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
	CirclingIndexes = SetupWolves(GetActorLocation(), GetActorRotation(), 2, 4);
	// ����� ���� ����
	float Angle = 0.0f;
	for (auto index : CirclingIndexes)
	{
		Wolves[index]->SetCirclingAngle(Angle);
		Angle += WolfInterval;
	}
	// CirclingWolvesDuration �ð� �� ���� ��ü�� �����
	GetWorldTimerManager().SetTimer(CircleWolvesTimer, this, &AShinbi::StopCircleWolves, CirclingWolvesDuration, false);

	bIsCircling = true;
}

void AShinbi::StopCircleWolves()
{
	for (auto Index : CirclingIndexes)
	{
		// ���� ����Ʈ ���� �� Disable
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
		const FVector TargetLoc = Target->GetActorLocation();
		float Angle = 0.0f;
		float TimeInterval = 0.001f;

		// Ÿ�̸Ӹ� ����Ͽ� �ð����� �ΰ� ���� ����
		for (auto& timer : SpawnWolvesTimer)
		{
			// Ÿ�̸� ����
			FTimerDelegate PrimaryTimerDel = FTimerDelegate::CreateUObject(this, &AShinbi::SetPrimaryWolves, TargetLoc, Angle);
			GetWorldTimerManager().SetTimer(timer, PrimaryTimerDel, TimeInterval, false);

			// ���� ������ ����, ���� �ð� ����
			Angle += PrimaryAngleInterval;
			TimeInterval += PrimarySpawnTime;
		}
	}
}

void AShinbi::SetPrimaryWolves(FVector NewLocation, float NewAngle)
{
	/* Ÿ���� �߽����� PrimaryAngle��ŭ ���� ��ġ�� ���� */
	FVector WolfLocation = NewLocation;
	const FVector RotationVec = FVector(250.0f, 0, 100.0f);
	const FVector Axis = FVector(0, 0, 1).GetSafeNormal();
	const FVector ResultVec = RotationVec.RotateAngleAxis(NewAngle, Axis);
	WolfLocation += ResultVec;

	// ���� Ȱ��ȭ
	TSet<int> IndexSet = SetupWolves(WolfLocation, FRotator::ZeroRotator, 3, 1);
	for (auto& index : IndexSet)
	{
		Wolves[index]->SetTargetLocation(NewLocation);
	}
}

TSet<int> AShinbi::SetupWolves(const FVector SpawnVec, const FRotator SpawnRot, uint8 Type, int SpawnNum)
{
	TSet<int> Indexes;
	// ���� ��������� ���� ���� ���� ����
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
		// �ε��� ��ȯ
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
