#include "ShinbiWolf.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Character/Shinbi/Shinbi.h"


// Sets default values
AShinbiWolf::AShinbiWolf()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentState = EWolfState::Idle;

	/* Circling Wolves ��ų ȸ�� ������ */
	RotSpeed = 180.0f;
	Radius = 300.0f;
	UpdateRot = 0.0f;
	StartAngle = 0.0f;

	AttackWolvesDuration = 1.5f;

	// Collision �浹 �Լ� ���ε�
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

void AShinbiWolf::Action(const EWolfState NewState)
{
	CurrentState = NewState;
	switch (CurrentState)
	{
	case EWolfState::Attack:
		UpdateFunc = &AShinbiWolf::StartAttackWolves;
		GetWorldTimerManager().SetTimer(AttackWolvesTimer, this, &AShinbiWolf::StopAttackWolves, AttackWolvesDuration, false);
		break;
	case EWolfState::Circle:
		UpdateFunc = &AShinbiWolf::StartCirclingWolves;
		break;
	case EWolfState::Ultimate:
		//SetupUltimate();
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
	SetActorHiddenInGame(false);
}

void AShinbiWolf::SetDisable()
{
	Action(EWolfState::Idle);
	SetActorHiddenInGame(true);
}

void AShinbiWolf::SetIsSpawn(bool bSpawn)
{
	bIsSpawn = bSpawn;
}

FRotator AShinbiWolf::LookAtTarget(FVector MyLocation, FVector TargetLocation) const
{
	const FVector DistanceVec = TargetLocation - MyLocation;
	const FRotator FocusRot = FRotationMatrix::MakeFromX(DistanceVec).Rotator();
	return FocusRot;
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

void AShinbiWolf::UpdateUltimate()
{
	// Ÿ�� �������� ȸ��
	const FRotator FocusRot = LookAtTarget(GetActorLocation(), TargetPawn->GetActorLocation());
	SetActorRelativeRotation(FocusRot);

	// �̵�
	if (bIsSpawn)
	{
		const FVector MovementVec = GetActorLocation() + GetActorForwardVector() * 8.0f;
		SetActorLocation(MovementVec);
	}
}

void AShinbiWolf::StartUltimate()
{
	// Ÿ�� �������� ��ƼŬ ����
	const FRotator FocusRot = LookAtTarget(GetActorLocation(), TargetPawn->GetActorLocation());
	FTransform ParticleTr = FTransform(FocusRot + FRotator(40.0f, 0, 0), GetActorLocation());
	UParticleSystemComponent* FXComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), UltHeartFX, ParticleTr);
	FXComp->CustomTimeDilation = 1.5f;

	PlayAnimMontage(LeapMontage);
	GetMesh()->SetHiddenInGame(false);
	UpdateFunc = &AShinbiWolf::UpdateUltimate;
}

void AShinbiWolf::StopUltimate()
{
	GetCharacterMovement()->GravityScale = 1.0f;
	bIsSpawn = false;
	SetDisable();
}

void AShinbiWolf::SetupUltimate(APawn* Target, const FVector NewLocation, float InRate)
{
	GetMesh()->SetHiddenInGame(true);
	GetCharacterMovement()->GravityScale = 0.0f;
	TargetPawn = Target;
	SetActorLocation(NewLocation);

	// Eye ��ƼŬ ����
	if (TargetPawn != nullptr)
	{
		const FRotator FocusRot = LookAtTarget(GetActorLocation(), TargetPawn->GetActorLocation());
		SetActorRelativeRotation(FocusRot);
		SpawnParticle(UltEyeFX);
	}
	// ���� �ð� �� Ultimate ��ų ���� �Լ� ȣ��
	GetWorldTimerManager().SetTimer(UltTimer, this, &AShinbiWolf::StartUltimate, InRate, false);
}

void AShinbiWolf::SetUltimateFinal(bool bFinal)
{
	bIsFinal = bFinal;
}

void AShinbiWolf::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UpdateFunc = nullptr;
}

void AShinbiWolf::BeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr &&
		OtherActor->GetClass() != this->GetClass())
	{
		if (CurrentState == EWolfState::Attack)
		{
			SpawnParticle(AttackWolvesImpactFX);
			SetDisable();
		}
		else if (CurrentState == EWolfState::Circle)
		{
			SpawnParticle(CirclingWolvesImpactFX);
		}
		else if (CurrentState == EWolfState::Ultimate)
		{
			// �浹 �˸�
			ACharacter* MyPlayer = UGameplayStatics::GetPlayerCharacter(this, 0);
			AShinbi* Shinbi = Cast<AShinbi>(MyPlayer);
			Shinbi->UltimateHitNotify();

			SpawnParticle(UltEndFX);
			StopUltimate();
		}
	}
}

