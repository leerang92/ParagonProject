// Fill out your copyright notice in the Description page of Project Settings.

#include "Shinbi.h"
#include "ShinbiWolf.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AShinbi::AShinbi()
{
	PrimaryActorTick.bCanEverTick = false;

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

void AShinbi::StartPrimary()
{
	// �������� �ƴ� ��
	if (!bAttacking)
	{
		bAttacking = true;
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
void AShinbi::AbilityMR()
{
	// Attack Wolves ��ų �ִϸ��̼� ��Ÿ�� ����
	PlayAnimMontage(AttackWolvesMontage);
}

void AShinbi::StopAttackCast()
{
	MainUMG->GetAbilityBar()->SetAbility(AbilityComp->GetAbilityInfo(1));
	// �ִ� ��Ÿ���� End ���� ���
	PlayAnimMontage(AttackWolvesMontage, 1.0f, TEXT("End"));

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

		PlayAnimMontage(DashMontage, 2.0f);
		SetCameraParticle(DashParticle); // ī�޶� �뽬 ����Ʈ ����

		// �������� �뽬
		LaunchCharacter(GetActorForwardVector() * DashPower, true, true);
	}
}

// Circling Wolves ��ų
void AShinbi::AbilityE()
{
	if (bIsCircling)
	{
		return;
	}

	Super::AbilityE();

	// �÷��̾� CircleWovles �ִϸ��̼� ����
	PlayAnimMontage(CircleWolvesMontage);
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

	bIsCircling = true;
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
	bIsCircling = false;
}

void AShinbi::Ultimate()
{
	TargetPawn = FocusView();
	if (TargetPawn != nullptr)
	{
		Super::Ultimate();

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

	// ���� �迭 ���� ���� �� �ʱ�ȭ
	//for (int i = 0; i < MaxWolfNum; ++i)
	//{
	//	Wolves[i]->SetLifeSpan(0.01f);
	//	Wolves[i] = nullptr;
	//}
	//Wolves.Empty();
}
