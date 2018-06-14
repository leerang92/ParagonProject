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
}

void AShinbi::BeginPlay()
{
	Super::BeginPlay();

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
	// �������� ��
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
		// ���� ��Ÿ�� �ִϸ��̼� ���
		PlayAnimMontage(AttackMontages[AttackCount++]);

		// ������ �����̸� ���� ī��Ʈ �ʱ�ȭ
		if (AttackMontages.Num() <= AttackCount)
		{
			AttackCount = 0;
		}
	}
}

void AShinbi::ResetComboAttack()
{
	//--AttackCount;
	//if (AttackCount < 0) { AttackCount = 0; }
	//PlayAnimMontage(AttackMontages[AttackCount], 1.0f, TEXT("Recovery"));

	Super::ResetComboAttack();
}

// Attack Wolves ��ų
void AShinbi::AbilityMouseR()
{
	// Attack Wolves ��ų �ִϸ��̼� ��Ÿ�� ����
	PlayAnimMontage(AttackWolvesMontage);
	IsAttacking = true;


	//UE_LOG(LogClass, Warning, TEXT("%d"), WolfsCount);
}

void AShinbi::StopAttackCast()
{
	// �ִ� ��Ÿ���� End ���� ���
	PlayAnimMontage(AttackWolvesMontage, 1.0f, TEXT("End"));

	// ���� ���� ����
	const FVector SpawnVec = GetActorLocation() + (GetActorForwardVector() * FVector(0, 0, -6.0f));
	SetupWolves(SpawnVec, GetActorRotation(), 1);
	//Wolves[WolfIndex]->SetEnable();
	//Wolves[WolfIndex]->Action(EWolfState::Attack);

	//AShinbiWolf* WolfActor = GetWorld()->SpawnActor<AShinbiWolf>(WolfClass, SpawnVec, GetActorRotation());
	//if (WolfActor)
	//{
	//	WolfActor->Action(EWolfState::Attack);
	//}
}

// Dash
void AShinbi::Ability1()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		PlayAnimMontage(DashMontage, 2.0f);
		SetCameraParticle(DashParticle); // ī�޶� �뽬 ����Ʈ ����

		// �������� �뽬
		//const FVector Velocity = GetVelocity().GetSafeNormal();
		LaunchCharacter(GetActorForwardVector() * DashPower, true, true);
	}
}

void AShinbi::Ability2()
{
	if (Wolves.Num() != 0)
		return;

	// �÷��̾� CircleWovles �ִϸ��̼� ����
	PlayAnimMontage(CircleWolvesMontage);

	// ����� ����
	float Angle = 0.0f;
	for (int i = 0; i < 4; ++i)
	{
		AShinbiWolf* WolfActor = GetWorld()->SpawnActor<AShinbiWolf>(WolfClass, GetActorLocation(), FRotator::ZeroRotator);
		if (WolfActor)
		{
			WolfActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			Wolves.Add(WolfActor);
			Wolves[i]->SetCirclingAngle(Angle);
			Wolves[i]->Action(EWolfState::Circle);
		}
		Angle += WolfInterval;
	}
	// CirclingWolvesDuration �ð� �� ���� ��ü�� �����
	GetWorldTimerManager().SetTimer(CircleWolvesTimer, this, &AShinbi::StopCircleWolves, CirclingWolvesDuration, false);
}

void AShinbi::PrimaryAbility()
{

}

void AShinbi::StopCircleWolves()
{
	for (auto wolves : Wolves)
	{
	/*	wolves->StopCirclingWovles();
		wolves->SetLifeSpan(0.01f);*/
	}
	Wolves.Empty();
}

void AShinbi::SetupWolves(const FVector SpawnVec, const FRotator SpawnRot, int SpawnNum)
{
	// ���� ��������� ���� ���� ���� ����
	for (int i = 0; i < SpawnNum;)
	{
		if (!Wolves[WolfIndex]->IsHiddenEd())
		{
			Wolves[WolfIndex]->SetEnable();
			Wolves[WolfIndex]->SetActorTransform(FTransform(SpawnRot, SpawnVec));
			Wolves[WolfIndex]->Action(EWolfState::Attack);
			++i;
		}
		// �ε��� ��ȯ
		++WolfIndex %= MaxWolfNum;
	}
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
