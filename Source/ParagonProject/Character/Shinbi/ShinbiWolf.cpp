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

	/* Circling Wolves 스킬 회전 변수들 */
	RotSpeed = 180.0f;
	Radius = 300.0f;
	UpdateRot = 0.0f;
	StartAngle = 0.0f;

	AttackWolvesDuration = 1.5f;

	// Collision 충돌 함수 바인드
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

void AShinbiWolf::UpdateUltimate()
{
	// 타겟 방향으로 회전
	const FRotator FocusRot = LookAtTarget(GetActorLocation(), TargetPawn->GetActorLocation());
	SetActorRelativeRotation(FocusRot);

	// 이동
	if (bIsSpawn)
	{
		const FVector MovementVec = GetActorLocation() + GetActorForwardVector() * 8.0f;
		SetActorLocation(MovementVec);
	}
}

void AShinbiWolf::StartUltimate()
{
	// 타겟 방향으로 파티클 생성
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

	// Eye 파티클 생성
	if (TargetPawn != nullptr)
	{
		const FRotator FocusRot = LookAtTarget(GetActorLocation(), TargetPawn->GetActorLocation());
		SetActorRelativeRotation(FocusRot);
		SpawnParticle(UltEyeFX);
	}
	// 일정 시간 후 Ultimate 스킬 시작 함수 호출
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
			// 충돌 알림
			ACharacter* MyPlayer = UGameplayStatics::GetPlayerCharacter(this, 0);
			AShinbi* Shinbi = Cast<AShinbi>(MyPlayer);
			Shinbi->UltimateHitNotify();

			SpawnParticle(UltEndFX);
			StopUltimate();
		}
	}
}

