// Copyright Epic Games, Inc. All Rights Reserved.

#include "Robot.h"
#include "ITTCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "Blueprint/UserWidget.h"
#include "DontForgetMeGameModeBase.h"

//////////////////////////////////////////////////////////////////////////
// ARobot

ARobot::ARobot()
{
	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 250.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// �⺻ �̵� �� ���� �ӵ� ����
	DefaultSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultJumpZVelocity = GetCharacterMovement()->JumpZVelocity;
	bIsEffectActive = false;

	// �ڱ��� �ð��� ǥ���� ���� StaticMeshComponent ���� �� ����
	MagneticField = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagneticField"));
	MagneticField->SetupAttachment(RootComponent);
	MagneticField->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MagneticField->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	MagneticField->SetVisibility(false);

	bIsInMagneticField = false;
	bIsMagneticFieldActive = false;


}
void ARobot::BeginPlay()
{
	Super::BeginPlay();

	//�⺻ ü�� ����
	CurrentHealth = MaxHealth;

	// �⺻ ���¹̳� ����
	CurrentStamina = MyMaxStamina;

	bIsStaminaDepleted = false;
	bIsRunning = false;

	GetWorld()->GetTimerManager().SetTimer(PossessCheckTimerHandle, this, &ARobot::CheckIfPossessed, 0.1f, false);

	MagneticField->OnComponentBeginOverlap.AddDynamic(this, &ARobot::OnFieldEnter);
	MagneticField->OnComponentEndOverlap.AddDynamic(this, &ARobot::OnFieldExit);

}

void ARobot::OnFieldEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AITTCharacter* OtherCharacter = Cast<AITTCharacter>(OtherActor);
	if (OtherCharacter)
	{
		OtherCharacter->IncreaseMovementSpeed();
	}
}

void ARobot::OnFieldExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AITTCharacter* OtherCharacter = Cast<AITTCharacter>(OtherActor);
	if (OtherCharacter)
	{
		//OtherCharacter->ResetMovementSpeed();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, OtherCharacter, &AITTCharacter::ResetMovementSpeed, 5.0f, false);
	}
}

void ARobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RecoverStamina(DeltaTime);

	UpdateHealth();
	UpdateStamina();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARobot::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ARobot::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ARobot::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ARobot::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ARobot::LookUpAtRate);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ARobot::StartRunning);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ARobot::StopRunning);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ARobot::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ARobot::TouchStopped);

	// EŰ �ڱ���
	PlayerInputComponent->BindAction("IncreaseEffect", IE_Pressed, this, &ARobot::IncreaseSpeedAndJump);
	PlayerInputComponent->BindAction("ToggleMagneticField", IE_Pressed, this, &ARobot::ToggleMagneticField);
	//PlayerInputComponent->BindAction("IncreaseEffect", IE_Pressed, this, &ARobot::ServerIncreaseSpeedAndJump);
}

void ARobot::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ARobot::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ARobot::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ARobot::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ARobot::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ARobot::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

// �޸���
void ARobot::StartRunning()
{
	if (CurrentStamina > 0 && !bIsRunning && !bIsStaminaDepleted)
	{
		bIsRunning = true;
		GetCharacterMovement()->MaxWalkSpeed = 600.0f; // �̵� �ӵ� ����
		// �޸��� ������ �� ���¹̳� ���� ����
		GetWorld()->GetTimerManager().SetTimer(StaminaDecreaseStaminaForRunning, this, &ARobot::DecreaseStaminaRunning, 0.1f, true);
	}
}

void ARobot::StopRunning()
{
	if (bIsRunning)
	{
		bIsRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = 250.0f; // �̵� �ӵ� ����
		// �޸��� ���� �� ���¹̳� ���� ����
		GetWorld()->GetTimerManager().ClearTimer(StaminaDecreaseStaminaForRunning);
	}
}

// �����÷��̾� �ߺ� ����
void ARobot::CheckIfPossessed()
{
	// �� ĳ���Ͱ� �ƹ� ��Ʈ�ѷ����� �������� �ʾҴٸ�
	if (GetController() == nullptr)
	{
		// ĳ���� ����
		Destroy();
	}
}


// EŰ �ڱ���
void ARobot::IncreaseSpeedAndJump()
{
	if (!bIsEffectActive)
	{
		bIsEffectActive = true;
		GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed * 1.5f;
		GetCharacterMovement()->JumpZVelocity = DefaultJumpZVelocity * 1.5f;
		MagneticField->SetVisibility(true);

		// �ڱ����� 1�� �Ŀ� ��Ȱ��ȭ
		GetWorld()->GetTimerManager().SetTimer(FieldVisibilityTimerHandle, this, &ARobot::HideMagneticField, 1.0f, false);

		// Ÿ�̸� �����Ͽ� 5�� �Ŀ� ȿ�� ����
		GetWorld()->GetTimerManager().SetTimer(EffectTimerHandle, this, &ARobot::ResetEffect, 5.0f, false);
	}

}

void ARobot::HideMagneticField()
{
	MagneticField->SetVisibility(false);
}

void ARobot::ResetEffect()
{
	NormalizeSpeedAndJump();
	bIsEffectActive = false;
}

void ARobot::NormalizeSpeedAndJump()
{
	GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed;
	GetCharacterMovement()->JumpZVelocity = DefaultJumpZVelocity;
	MagneticField->SetVisibility(false);
}

void ARobot::ToggleMagneticField()
{
	if (!MagneticField->IsCollisionEnabled())
	{
		MagneticField->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		// 5�� �Ŀ� �ڵ����� NoCollision���� �����ϴ� Ÿ�̸� ����
		GetWorld()->GetTimerManager().SetTimer(MagneticFieldTimerHandle, this, &ARobot::DisableMagneticField, 1.0f, false);
	}
	else
	{
		// ��� ��Ȱ��ȭ, Ÿ�̸Ӹ� ���
		DisableMagneticField();
	}
}

void ARobot::DisableMagneticField()
{
	if (MagneticField->IsCollisionEnabled())
	{
		MagneticField->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Ȥ�� �𸣴� Ÿ�̸Ӹ� ���
		GetWorld()->GetTimerManager().ClearTimer(MagneticFieldTimerHandle);
	}
}

void ARobot::CheckMagneticField()
{
	bIsMagneticFieldActive = !bIsMagneticFieldActive; // �ڱ��� Ȱ��ȭ ���� ���
}

bool ARobot::IsMagneticFieldActive() const
{
	return bIsMagneticFieldActive;
}

//�÷��̾� �浹 ���� üũ
FVector ARobot::GetForwardLocation(float Distance)
{
	FVector ForwardVector = GetActorForwardVector(); // ���Ͱ� �ٶ󺸴� ����
	ForwardVector *= Distance;
	ForwardVector += GetActorLocation();

	return ForwardVector;
}

void ARobot::Interactive()
{
	FVector StartLocation = GetForwardLocation(50.0f);
	FVector EndLocation = GetForwardLocation(100.0f);
	if (CheckForObjectsInChannel(StartLocation, EndLocation, ECC_GameTraceChannel2, LocalCollisionShape))
	{
		InteractableActor = Cast<IInterfaceInteractable>(OutHit.GetActor());
		if (InteractableActor)
		{
			InteractableActor->Interact(MyCharacter);
		}
	}
}

bool ARobot::CheckForObjectsInChannel(FVector StartLocation, FVector EndLocation, ECollisionChannel CollisionChannel, FCollisionShape CollisionShape)
{
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(this); // �ڱ� �ڽ��� �浹���� ����

	// SweepSingleByChannel�� ����Ͽ� ������ ������ ä�ο��� ������Ʈ�� �ִ��� Ȯ��
	bool bIsObjectFound = GetWorld()->SweepSingleByChannel(
		OutHit,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		CollisionChannel,
		CollisionShape,
		QueryParams
	);

	return bIsObjectFound && OutHit.bBlockingHit; // �浹�� �߻��ߴ��� ���θ� ��ȯ
}

void ARobot::CreateHealthBar()
{
	auto PlayerController = Cast<APlayerController>(GetController());
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);

	if (PlayerController != nullptr && PlayerHealthBarClass != nullptr && CurrentLevelName != "Mainmenu")
	{
		PlayerHealthBar = CreateWidget<UUserWidget>(PlayerController, PlayerHealthBarClass);
		if (PlayerHealthBar)
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerHealthBar Suc"));
			PlayerHealthBar->AddToViewport(0);
			PlayerHealthBar->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Error, TEXT("Player ID: %d"), PlayerController->PlayerState ? PlayerController->PlayerState->GetPlayerId() : -1);

			// ��� �÷��̾� ��Ʈ�ѷ��� ��ȸ�Ͽ� ���� ���� PlayerId ã��
			int32 LowestPlayerId = INT32_MAX; // ���� ū int ������ �ʱ�ȭ
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				APlayerController* IterPlayerController = Iterator->Get();
				if (IterPlayerController && IterPlayerController->PlayerState)
				{
					int32 PlayerId = IterPlayerController->PlayerState->GetPlayerId();
					if (PlayerId < LowestPlayerId)
					{
						LowestPlayerId = PlayerId;
					}
				}
			}

			// ���� PlayerController�� PlayerId
			int32 CurrentPlayerId = PlayerController->PlayerState ? PlayerController->PlayerState->GetPlayerId() : -1;

			// UI ��ġ ����
			float NewX, NewY;
			FVector2D ViewportSize;
			if (GEngine && GEngine->GameViewport)
			{
				GEngine->GameViewport->GetViewportSize(ViewportSize);
				// ���� �÷��̾��� ���
				if (CurrentPlayerId == LowestPlayerId)
				{
					NewX = (-(ViewportSize.X / 2.0f)) - 250;
					UE_LOG(LogTemp, Error, TEXT("NewX : %f"), ViewportSize.X);
					NewY = ViewportSize.Y - (ViewportSize.Y / 5.f);
				}
				else // ���߿� �߰��� �÷��̾��� ���
				{
					NewX = (ViewportSize.X / 2.0f) - 250; // ȭ���� ������ �߰�
					NewY = ViewportSize.Y - (ViewportSize.Y / 5.f);
				}

				FVector2D NewPosition(NewX, NewY);

				// VerticalBox�� CanvasPanelSlot�� ã�� ��ġ�� �����մϴ�.
				UVerticalBox* MyVerticalBox = Cast<UVerticalBox>(PlayerHealthBar->GetWidgetFromName(TEXT("MyVerticalBox")));
				if (MyVerticalBox)
				{
					UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MyVerticalBox->Slot);
					if (CanvasSlot)
					{
						CanvasSlot->SetPosition(NewPosition);
					}
				}
			}
		}
	}
}

//ü�� �� ���¹̳� ����
void ARobot::UpdateHealth()
{
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);


	if (PlayerHealthBar)
	{
		UProgressBar* HealthBar = Cast<UProgressBar>(PlayerHealthBar->GetWidgetFromName(TEXT("ProgressBar_HP")));
		if (HealthBar)
		{
			HealthBar->SetPercent(CurrentHealth / MaxHealth);
		}
	}
}

void ARobot::UpdateStamina()
{
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, MyMaxStamina);

	if (PlayerHealthBar)
	{
		UProgressBar* StaminaBar = Cast<UProgressBar>(PlayerHealthBar->GetWidgetFromName(TEXT("ProgressBar_stamina"))); // ProgressBar_Stamina�� ���¹̳� ���� �̸��Դϴ�.
		if (StaminaBar)
		{
			StaminaBar->SetPercent(CurrentStamina / MyMaxStamina);

			if (CurrentStamina <= 1.0f)
			{
				FLinearColor NewColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // RGB ���� �ڵ� + ����(����) ��
				StaminaBar->SetFillColorAndOpacity(NewColor);
			}
			if (CurrentStamina > 1.0f)
			{
				FLinearColor NewColor = FLinearColor(0.5f, 1.0f, 0.27f, 1.0f); // RGB ���� �ڵ� + ����(����) ��
				StaminaBar->SetFillColorAndOpacity(NewColor);
			}
		}
	}
}

void ARobot::DecreaseStamina(float StaminaAmount)
{

	CurrentStamina -= StaminaAmount; // ��: �޸���� ���� ���¹̳� ����
	if (CurrentStamina <= 0.2)
	{
		CurrentStamina = 0;
		bIsStaminaDepleted = true;
		StopRunning(); // ���¹̳ʰ� �� �����Ǹ� �޸��� ����
		// ���ο� ���·� ��ȯ
		if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
		{
			CharMovement->MaxWalkSpeed /= 2; // �̵� �ӵ��� �������� ����
		}
		GetWorld()->GetTimerManager().SetTimer(SlowStateTimer, this, &ARobot::RestoreWalkSpeed, SlowDuration, false);
	}
}

void ARobot::DecreaseStaminaRunning()
{
	DecreaseStamina(0.1f);
}

void ARobot::RecoverStamina(float DeltaTime)
{
	if (!bIsRunning && CurrentStamina < MyMaxStamina)
	{
		float RecoveryRate = bIsStaminaDepleted ? SlowStaminaRecoveryRate : StaminaRecoveryRate;
		CurrentStamina += RecoveryRate * DeltaTime;
		CurrentStamina = FMath::Min(CurrentStamina, MyMaxStamina);
	}
}

void ARobot::RestoreWalkSpeed()
{
	if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
	{
		if (bIsStaminaDepleted && CurrentStamina >= 1)
		{
			CharMovement->MaxWalkSpeed *= 2; // �̵� �ӵ��� ������� ����
			bIsStaminaDepleted = false;
		}
	}
}

float ARobot::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CurrentHealth -= ActualDamage;

	UpdateHealth();
	if (CurrentHealth <= 0.f)
	{
		//	Die();
	}
	return ActualDamage;
}


void ARobot::IncreaseMovementSpeed()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp && !bIsSpeedBoosted) // �ӵ� ������ �̹� ����� ���°� �ƴ� ��쿡�� ����
	{
		MovementComp->MaxWalkSpeed *= 1.5f;  // �̵� �ӵ� ����
		MovementComp->JumpZVelocity *= 1.5f; // ���� �ӵ� ����
		bIsSpeedBoosted = true;
	}
}

void ARobot::ResetMovementSpeed()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp && bIsSpeedBoosted)
	{
		MovementComp->MaxWalkSpeed /= 1.5f;  // �̵� �ӵ� ���� ����
		MovementComp->JumpZVelocity /= 1.5f; // ���� �ӵ� ���� ����
		bIsSpeedBoosted = false;
	}
}