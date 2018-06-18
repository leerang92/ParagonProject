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

	/* Circling Wolves 스킬 회전 변수들 */
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

	// 기본 컨트롤러 생성 후 장착
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
	// 파티클이 있고 액터가 숨겨지지 않았다면
	if (AttackWolvesEndFX != nullptr && IsHiddenEd())
	{
		// 파티클 생성 후 객체 제거
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
		// 늑대 위치 지정
		const FVector PlayerLoc = MyPlayer->GetActorLocation();
		const FVector InitLocation = RotateActorPoint(PlayerLoc, Radius, StartAngle, UpdateRot);
		SetActorLocation(InitLocation);

		// 늑대가 플레이어를 봐라보도록 회전
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
	// Radius 떨어진 곳에 늑대가 회전할 위치
	const FVector RotationVec = FVector(Radius, 0, -50.0f);
	// Z축 회전
	const FVector Axis = FVector(0, 0, 1).GetSafeNormal();
	// 타겟을 기준으로 회전할 벡터 구함
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

