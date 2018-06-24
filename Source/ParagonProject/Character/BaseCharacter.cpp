// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TraceDistance = 2000.0f;
	AttackCount = 0;
	SaveCombo = true;

	// 스킬 함수들 함수 포인터 배열에 바인드
	AbilityFuncs.Add(&ABaseCharacter::AbilityMouseR);
	AbilityFuncs.Add(&ABaseCharacter::Ability1);
	AbilityFuncs.Add(&ABaseCharacter::Ability2);
	AbilityFuncs.Add(&ABaseCharacter::Ultimate);
		
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	/* 컨트롤러 설정 */
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	/* 스프링 카메라 컴포넌트 */
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Arm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = 450.0f;
	CameraArm->bUsePawnControlRotation = false;

	/* 카메라 컴포넌트 */
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Comp"));
	CameraComp->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = true;

	/* 캐릭터 무브먼트 컴포넌트 */
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	/* 카메라 정면의 이펙트 */
	CamParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Ablity Paritcle"));
	CamParticle->SetupAttachment(CameraComp);

	AbilityComp = CreateDefaultSubobject<UAbilityComponent>(TEXT("Ability Component"));
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (MainUMGClass != nullptr)
	{
		MainUMG = CreateWidget<UMainUMG>(GetWorld(), MainUMGClass);
		MainUMG->AddToViewport();
		
		SetAbilityBar();
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/* 이동 */
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABaseCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABaseCharacter::LookUpAtRate);

	/* 공격 */
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ABaseCharacter::StartAttack);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ABaseCharacter::StopAttack);

	/* 스킬 */
	PlayerInputComponent->BindAction("AbilityMouseR", IE_Pressed, this, &ABaseCharacter::UseAbility<0>);
	PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &ABaseCharacter::UseAbility<1>);
	PlayerInputComponent->BindAction("Ability2", IE_Pressed, this, &ABaseCharacter::UseAbility<2>);
	PlayerInputComponent->BindAction("Ultimate", IE_Pressed, this, &ABaseCharacter::UseAbility<3>);
}


APawn * ABaseCharacter::FocusView()
{
	FVector CamLoc;
	FRotator CamRot;

	// 카메라 위치와 회전값을 통해 트레이서 발사 위치 구함
	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector EndLoc = CamLoc + (CamRot.Vector() * TraceDistance);

	// 트레이스 발사
	FCollisionQueryParams TraceParam(TEXT("TracePlayerView"), true, this);
	TraceParam.bTraceAsyncScene = true;
	TraceParam.bReturnPhysicalMaterial = false;
	TraceParam.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, EndLoc, ECC_Pawn, TraceParam);

	return Cast<APawn>(Hit.GetActor());
}

void ABaseCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABaseCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABaseCharacter::SetCameraParticle(UParticleSystem * NewParticle)
{
	if (NewParticle)
	{
		CamParticle->SetTemplate(NewParticle);
	}
}

void ABaseCharacter::StartAttack()
{

}

void ABaseCharacter::ComboAttack()
{

}

void ABaseCharacter::StopAttack()
{
	SaveCombo = false;
}

// 공격 변수들 초기화
void ABaseCharacter::ResetComboAttack()
{
	SaveCombo = true;
	IsAttacking = false;
	AttackCount = 0;
}

template<int T>
void ABaseCharacter::UseAbility()
{
	ResetComboAttack();
	StopAttack();
	(this->*(AbilityFuncs[T]))();
}

void ABaseCharacter::SetAbilityBar()
{
	// 스킬 아이콘 설정
	uint8 Index = 0;
	for (int i = 0; i < 5; ++i)
	{
		FAbilityInfo Info = AbilityComp->GetAbilityInfo(i);
		MainUMG->GetAbilityBar()->SetImage(Info.Image, i);
	}
}

/* 스킬 함수들 */
void ABaseCharacter::Ultimate()
{
	MainUMG->GetAbilityBar()->SetAbility(AbilityComp->GetAbilityInfo(static_cast<int>(EAbilityType::Ultimate)));
}
void ABaseCharacter::Ability1()
{
	MainUMG->GetAbilityBar()->SetAbility(AbilityComp->GetAbilityInfo(static_cast<int>(EAbilityType::Ability1)));
}
void ABaseCharacter::Ability2()
{
	MainUMG->GetAbilityBar()->SetAbility(AbilityComp->GetAbilityInfo(static_cast<int>(EAbilityType::Ability2)));
}
void ABaseCharacter::AbilityMouseR()
{
	MainUMG->GetAbilityBar()->SetAbility(AbilityComp->GetAbilityInfo(static_cast<int>(EAbilityType::MouseR)));
}