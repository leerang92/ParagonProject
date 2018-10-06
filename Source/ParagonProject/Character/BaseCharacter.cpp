// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TraceDistance	= 2000.0f;
	SectionCount	= 0;
	bSaveCombo		= true;

	// Register attack method
	AbilityFuncs.Add(&ABaseCharacter::AbilityMR);
	AbilityFuncs.Add(&ABaseCharacter::AbilityQ);
	AbilityFuncs.Add(&ABaseCharacter::AbilityE);
	AbilityFuncs.Add(&ABaseCharacter::Ultimate);
		
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	/* Setup Camera */
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Arm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = 450.0f;
	CameraArm->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Comp"));
	CameraComp->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	/* Setup particle to camera */
	CamParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Ablity Paritcle"));
	CamParticle->SetupAttachment(CameraComp);

	AbilityComp = CreateDefaultSubobject<UAbilityComponent>(TEXT("Ability Component"));
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Spawn main UMG
	if (MainUMGClass != nullptr)
	{
		MainUMG = CreateWidget<UMainUMG>(GetWorld(), MainUMGClass);
		
		if (MainUMG != nullptr)
		{
			MainUMG->AddToViewport();
			// Setup Ability Icons in Ability Bar
			MainUMG->SetupAbilityBar(*AbilityComp);
		}
		else
		{
			UE_LOG(LogClass, Warning, TEXT("Null Reperence Main UMG class"));
		}
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

	/* Movement */
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABaseCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABaseCharacter::LookUpAtRate);

	/* Attack and Ability */
	PlayerInputComponent->BindAction("Primary", IE_Pressed, this, &ABaseCharacter::StartPrimary);
	PlayerInputComponent->BindAction("Primary", IE_Released, this, &ABaseCharacter::StopPrimary);
	PlayerInputComponent->BindAction("AbilityMR", IE_Pressed, this, &ABaseCharacter::OnAbility<0>);
	PlayerInputComponent->BindAction("AbilityQ", IE_Pressed, this, &ABaseCharacter::OnAbility<1>);
	PlayerInputComponent->BindAction("AbilityE", IE_Pressed, this, &ABaseCharacter::OnAbility<2>);
	PlayerInputComponent->BindAction("Ultimate", IE_Pressed, this, &ABaseCharacter::OnAbility<3>);
}

APawn * ABaseCharacter::FocusView()
{
	FVector CamLoc;
	FRotator CamRot;

	// Locate the tray launch via camera position and rotation value
	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector EndLoc = CamLoc + (CamRot.Vector() * TraceDistance);

	// Shot raytrace
	FCollisionQueryParams TraceParam(TEXT("TracePlayerView"), true, this);
	TraceParam.bTraceAsyncScene = true;
	TraceParam.bReturnPhysicalMaterial = false;
	TraceParam.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, EndLoc, ECC_Pawn, TraceParam);

	return Cast<APawn>(Hit.GetActor());
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

void ABaseCharacter::SetCameraParticle(UParticleSystem * NewParticle)
{
	if (NewParticle)
	{
		CamParticle->SetTemplate(NewParticle);
	}
}

void ABaseCharacter::SetMouseCenterLocation()
{
	UGameViewportClient* Viewport = GetWorld()->GetGameViewport();
	FVector2D Center;
	Viewport->GetViewportSize(Center);

	APlayerController* PCon = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PCon->SetMouseLocation(Center.X * 0.5f, Center.Y * 0.5f);
}

void ABaseCharacter::StartPrimary()
{
	// 공격중이 아닐 시
	if (!bAttacking)
	{
		bAttacking = true;
		bSaveCombo = true;
		ComboAttack();
	}
	// 이미 공격중일 시 콤보 이어하기
	else if (!bSaveCombo)
	{
		bSaveCombo = true;
	}
}

void ABaseCharacter::ComboAttack()
{
	if (MainUMG)
	{
		const FAbilityInfo AbilityInfo = AbilityComp->GetAbilityInfo(EAbilityType::Primary);
		MainUMG->GetAbilityBar()->SetAbilityUI(AbilityInfo);
	}
}

void ABaseCharacter::StopPrimary()
{
	bSaveCombo = false;
}

void ABaseCharacter::ResetComboAttack()
{
	bAttacking = false;
	bSaveCombo = true;
	SectionCount = 0;
}

template<int T>
void ABaseCharacter::OnAbility()
{
	ResetComboAttack();
	StopPrimary();

	if (MainUMG != nullptr)
	{
		(this->*(AbilityFuncs[T]))();
	}
}

/* 스킬 함수들 */
void ABaseCharacter::AbilityMR()
{
	MainUMG->GetAbilityBar()->SetAbilityUI(AbilityComp->GetAbilityInfo(1));
}
void ABaseCharacter::AbilityQ()
{
	MainUMG->GetAbilityBar()->SetAbilityUI(AbilityComp->GetAbilityInfo(2));
}
void ABaseCharacter::AbilityE()
{
	MainUMG->GetAbilityBar()->SetAbilityUI(AbilityComp->GetAbilityInfo(3));
}
void ABaseCharacter::Ultimate()
{
	MainUMG->GetAbilityBar()->SetAbilityUI(AbilityComp->GetAbilityInfo(4));
}

void ABaseCharacter::OnRangeDamage(FVector & Origin, float BaseDamage, float DamageRadius)
{
	TArray<AActor*> IgnoreActor;
	bool bDamage = UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, Origin, DamageRadius, UDamageType::StaticClass(), IgnoreActor, this);
	if (bDamage)
	{
		UE_LOG(LogClass, Warning, TEXT("Set Damage"));
	}
}
