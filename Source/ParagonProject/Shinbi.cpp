// Fill out your copyright notice in the Description page of Project Settings.

#include "Shinbi.h"
#include "ShinbiWolf.h"
#include "Runtime/Engine/Public/TimerManager.h"

AShinbi::AShinbi()
{
	DashPower = 4500.0f;
	WolfInterval = 90.0f;
	WolfIndex = 0;
	CirclingWolvesDuration = 7.0f;
}

void AShinbi::BeginPlay()
{
	Super::BeginPlay();
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

// Attack Wolves ��ų
void AShinbi::AbilityMouseR()
{
	// Attack Wolves ��ų �ִϸ��̼� ��Ÿ�� ����
	PlayAnimMontage(AttackWolvesMontage);
	IsAttacking = true;
}

// Dash
void AShinbi::Ability1()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		PlayAnimMontage(DashMontage, 2.0f);
		SetCameraParticle(DashParticle); // ī�޶� �뽬 ����Ʈ ����

		// �������� �뽬
		const FVector Velocity = GetVelocity().GetSafeNormal();
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
			/*Wolves[i]->SetCirclingAngle(Angle);
			Wolves[i]->Action(EWolfState::Circle);*/
		}
		Angle += WolfInterval;
	}
	// CirclingWolvesDuration �ð� �� ���� ��ü�� �����
	GetWorldTimerManager().SetTimer(CircleWolvesTimer, this, &AShinbi::StopCircleWolves, CirclingWolvesDuration, false);
}

void AShinbi::PrimaryAbility()
{

}

void AShinbi::StopAttackCast()
{
	// �ִ� ��Ÿ���� End ���� ���
	PlayAnimMontage(AttackWolvesMontage, 1.0f, TEXT("End"));

	// ���� ���� ����
	const FVector SpawnVec = GetActorLocation() + GetActorForwardVector() - FVector(-100.0f, 0, 80.0f);
	AShinbiWolf* WolfActor = GetWorld()->SpawnActor<AShinbiWolf>(WolfClass, SpawnVec, GetActorRotation());
	if (WolfActor)
	{
		//WolfActor->Action(EWolfState::Attack);
	}
}

void AShinbi::StopCircleWolves()
{
	for (auto wolves : Wolves)
	{
		//wolves->StopCirclingWovles();
		//wolves->SetLifeSpan(0.01f);
	}
	Wolves.Empty();
}
