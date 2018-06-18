#include "ShinbiWolf.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"

// Sets default values
AShinbiWolf::AShinbiWolf()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentState = EWolfState::Idle;

	/* Circling Wolves ��ų ȸ�� ������ */
	RotSpeed = 180.0f;
	Radius = 300.0f;
	UpdateRot = 0.0f;
	StartAngle = 0.0f;

	AttackWolvesDuration = 1.5f;

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AShinbiWolf::BeginOverlap);
}

// Called when the game starts or when spawned
void AShinbiWolf::BeginPlay()
{
	Super::BeginPlay();

	// �⺻ ��Ʈ�ѷ� ���� �� ����
	SpawnDefaultController();
}

// Called every frame
void AShinbiWolf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UpdateFunc != nullptr)
	{
		(this->*(UpdateFunc))();
	}
}

void AShinbiWolf::Action(const EWolfState NewState, const FVector InitVec, const FRotator InitRot)
{
	CurrentState = NewState;
	//SetActorTransform(FTransform(InitRot, InitVec));
	switch (CurrentState)
	{
	case EWolfState::Attack:
		UpdateFunc = &AShinbiWolf::StartAttackWolves;
		GetWorldTimerManager().SetTimer(AttackWolvesTimer, this, &AShinbiWolf::StopAttackWolves, AttackWolvesDuration, false);
		break;
	case EWolfState::Circle:
		UpdateFunc = &AShinbiWolf::StartCirclingWolves;
		break;
	case EWolfState::Primary:
		GetCharacterMovement()->GravityScale = 0.0f;
		PlayAnimMontage(LeapMontage);
		UpdateFunc = &AShinbiWolf::StartPrimary;
		break;
	}
}

void AShinbiWolf::SetCirclingAngle(float NewAngle)
{
	StartAngle = NewAngle;
}

void AShinbiWolf::SpawnParticle(UParticleSystem * NewFX)
{
	if (NewFX == nullptr)
	{
		return;
	}
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), NewFX, GetActorTransform());
}

void AShinbiWolf::SetEnable()
{
	bIsEnable = true;
	SetActorHiddenInGame(false);
}

void AShinbiWolf::SetDisable()
{
	bIsEnable = false;
	Action(EWolfState::Idle);
	SetActorHiddenInGame(true);
}

void AShinbiWolf::SetIsSpawn(bool bSpawn)
{
	bIsSpawn = bSpawn;
}

void AShinbiWolf::StartAttackWolves()
{
	AddMovementInput(GetActorForwardVector());
}

void AShinbiWolf::StopAttackWolves()
{
	// ��ƼŬ�� �ְ� ���Ͱ� �������� �ʾҴٸ�
	if (AttackWolvesEndFX != nullptr && IsHiddenEd())
	{
		// ��ƼŬ ���� �� ��ü ����
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AttackWolvesEndFX, GetActorTransform());
	}
	SetDisable();
}

void AShinbiWolf::StartCirclingWolves()
{
	if (UpdateRot > 360.0f)
	{
		UpdateRot = 0.0f;
	}
	UpdateRot += RotSpeed * GetWorld()->DeltaTimeSeconds;

	ACharacter* MyPlayer = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (MyPlayer)
	{
		// ���� ��ġ ����
		const FVector PlayerLoc = MyPlayer->GetActorLocation();
		const FVector InitLocation = RotateActorPoint(PlayerLoc, Radius, StartAngle, UpdateRot);
		SetActorLocation(InitLocation);

		// ���밡 �÷��̾ ���󺸵��� ȸ��
		const FVector TargetLoc = (GetActorLocation() - PlayerLoc);
		FRotator FocusRot = FRotationMatrix::MakeFromZ(TargetLoc).Rotator();
		FocusRot += FRotator(0, 0, 100.0f);
		SetActorRelativeRotation(FocusRot);
	}
}

void AShinbiWolf::StopCirclingWolves()
{
	SpawnParticle(CirclingRemovalFX);
	SetDisable();
}

void AShinbiWolf::SetTargetLocation(const FVector & TargetLoc)
{
	TargetLocation = TargetLoc;
}

FVector AShinbiWolf::RotateActorPoint(FVector TargetLocation, const float Radius, const float Angle, const float RotationRate)
{
	// Radius ������ ���� ���밡 ȸ���� ��ġ
	const FVector RotationVec = FVector(Radius, 0, -50.0f);
	// Z�� ȸ��
	const FVector Axis = FVector(0, 0, 1).GetSafeNormal();
	// Ÿ���� �������� ȸ���� ���� ����
	const FVector ResultVec = RotationVec.RotateAngleAxis((RotationRate + Angle), Axis);
	TargetLocation += ResultVec;

	return TargetLocation;
}

void AShinbiWolf::StartPrimary()
{
	const FVector DistanceVec = TargetLocation - GetActorLocation();
	const FRotator FocusRot = FRotationMatrix::MakeFromX(DistanceVec).Rotator();
	SetActorRelativeRotation(FocusRot);

	if (bIsSpawn)
	{
		const FVector MovementVec = GetActorLocation() + GetActorForwardVector() * 8.0f;
		SetActorLocation(MovementVec);
	}
}

void AShinbiWolf::StopPrimary()
{
	GetCharacterMovement()->GravityScale = 1.0f;
}

void AShinbiWolf::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UpdateFunc = nullptr;
}

void AShinbiWolf::BeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ACharacter* MyPlayer = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr &&
		OtherActor->GetClass() != this->GetClass())
	{
		if (CurrentState == EWolfState::Attack || CurrentState == EWolfState::Primary)
		{
			SpawnParticle(AttackWolvesImpactFX);
			SetDisable();
		}
		else if (CurrentState == EWolfState::Circle)
		{
			SpawnParticle(CirclingWolvesImpactFX);
		}
	}
}

