// Copyright Epic Games, Inc. All Rights Reserved.

#include "ITTCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerState.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "DontForgetMeGameModeBase.h"


//////////////////////////////////////////////////////////////////////////
// AITTCharacter

AITTCharacter::AITTCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

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
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	//CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	////// Create a follow camera
	//FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

}

void AITTCharacter::BeginPlay()
{
	Super::BeginPlay();


	//�⺻ ü�� ����
	CurrentHealth = MaxHealth;

	// �⺻ ���¹̳� ����

	CurrentStamina = MaxStamina;

	bIsStaminaDepleted = false;
	bIsRunning = false;

	CheckForController();

	LocalCollisionShape = FCollisionShape::MakeSphere(80.0f); //�浹 ���� 50.0f ����

	UE_LOG(LogTemp, Error, TEXT("RespawnTest"));

	//GetWorld()->GetTimerManager().SetTimer(StaminaRecoveryTimer, this, &AITTCharacter::RecoverStamina, NormalRecoveryTime, true);

	GetWorld()->GetTimerManager().SetTimer(PossessCheckTimerHandle, this, &AITTCharacter::CheckIfPossessed, 0.3f, false);

	UpdateHealth();
}

void AITTCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RecoverStamina(DeltaTime);

	UpdateHealth();
	UpdateStamina();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AITTCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AITTCharacter::CheckJumpStamina);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AITTCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AITTCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AITTCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AITTCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AITTCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AITTCharacter::TouchStopped);

	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &AITTCharacter::Interactive);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AITTCharacter::StartRunning);

	PlayerInputComponent->BindAction("Run", IE_Released, this, &AITTCharacter::StopRunning);

	MyCharacter = this;
}

void AITTCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AITTCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AITTCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AITTCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AITTCharacter::MoveForward(float Value)
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

void AITTCharacter::MoveRight(float Value)
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

//�÷��̾� ��Ʈ�ѷ� �� Possess üũ
void AITTCharacter::CheckForController()
{
	if (GetController() != nullptr)
	{
		// ��Ʈ�ѷ��� ���� ��� ���� ���� ���� ����
		CreateHealthBar();
	}
	else
	{
		// ��Ʈ�ѷ��� ���� ��� 0.01�� �� �ٽ� Ȯ��
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AITTCharacter::CheckForController, 0.01f, false);
	}
}

void AITTCharacter::CheckIfPossessed()
{
	// �� ĳ���Ͱ� �ƹ� ��Ʈ�ѷ����� �������� �ʾҴٸ�
	if (GetController() == nullptr)
	{
		// ĳ���� ����
		Destroy();
	}
}

//�÷��̾� �浹 ���� üũ
FVector AITTCharacter::GetForwardLocation(float Distance)
{
	FVector ForwardVector = GetActorForwardVector(); // ���Ͱ� �ٶ󺸴� ����
	ForwardVector *= Distance;
	ForwardVector += GetActorLocation();

	return ForwardVector;
}

bool AITTCharacter::CheckForObjectsInChannel(FVector StartLocation, FVector EndLocation, ECollisionChannel CollisionChannel, FCollisionShape CollisionShape)
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

void AITTCharacter::CreateHealthBar()
{
	auto PlayerController = Cast<APlayerController>(GetController());
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);

	if (PlayerController != nullptr && PlayerHealthBarClass != nullptr && CurrentLevelName != "Mainmenu")
	{
		PlayerHealthBar = CreateWidget<UUserWidget>(PlayerController, PlayerHealthBarClass);
		PlayerRespawn = CreateWidget<UUserWidget>(PlayerController, PlayerRespawnClass);
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
					NewY = ViewportSize.Y - (ViewportSize.Y / 3.f);
				}
				else // ���߿� �߰��� �÷��̾��� ���
				{
					NewX = (ViewportSize.X / 2.0f) - 250; // ȭ���� ������ �߰�
					NewY = ViewportSize.Y - (ViewportSize.Y / 3.f);
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
void AITTCharacter::UpdateHealth()
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

void AITTCharacter::UpdateStamina()
{
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, MaxStamina);

	if (PlayerHealthBar)
	{
		UProgressBar* StaminaBar = Cast<UProgressBar>(PlayerHealthBar->GetWidgetFromName(TEXT("ProgressBar_stamina"))); // ProgressBar_Stamina�� ���¹̳� ���� �̸��Դϴ�.
		if (StaminaBar)
		{
			StaminaBar->SetPercent(CurrentStamina / MaxStamina);

			if (CurrentStamina <= 1.0f)
			{
				FLinearColor NewColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // RGB ���� �ڵ� + ����(�����) ��
				StaminaBar->SetFillColorAndOpacity(NewColor);
			}
			if (CurrentStamina > 1.0f)
			{
				FLinearColor NewColor = FLinearColor(0.5f, 1.0f, 0.27f, 1.0f); // RGB ���� �ڵ� + ����(�����) ��
				StaminaBar->SetFillColorAndOpacity(NewColor);
			}
		}
	}
}

void AITTCharacter::DecreaseStamina(float StaminaAmount)
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
		GetWorld()->GetTimerManager().SetTimer(SlowStateTimer, this, &AITTCharacter::RestoreWalkSpeed, SlowDuration, false);
	}
}

void AITTCharacter::DecreaseStaminaRunning()
{
	DecreaseStamina(0.1f);
}

void AITTCharacter::RecoverStamina(float DeltaTime)
{
	if (!bIsRunning && CurrentStamina < MaxStamina)
	{
		float RecoveryRate = bIsStaminaDepleted ? SlowStaminaRecoveryRate : StaminaRecoveryRate;
		CurrentStamina += RecoveryRate * DeltaTime;
		CurrentStamina = FMath::Min(CurrentStamina, MaxStamina);
	}
}

void AITTCharacter::RestoreWalkSpeed()
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

void AITTCharacter::CheckJumpStamina()
{
	if (CurrentStamina >= 1.0f && JumpIsAllowedInternal())
	{
		ACharacter::Jump();
		//DecreaseStamina(1.0f);
	}
}

float AITTCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CurrentHealth -= ActualDamage;

	UpdateHealth();
	if (CurrentHealth <= 0.f)
	{
		Die();
	}
	return ActualDamage;
}

//�޸��� �� ��ȣ�ۿ�

void AITTCharacter::StartRunning()
{
	if (CurrentStamina > 0 && !bIsRunning && !bIsStaminaDepleted)
	{
		bIsRunning = true;
		GetCharacterMovement()->MaxWalkSpeed = 600.0f; // �̵� �ӵ� ����
		// �޸��� ������ �� ���¹̳� ���� ����
		GetWorld()->GetTimerManager().SetTimer(StaminaDecreaseStaminaForRunning, this, &AITTCharacter::DecreaseStaminaRunning, 0.1f, true);
	}
}

void AITTCharacter::StopRunning()
{
	if (bIsRunning)
	{
		bIsRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = 250.0f; // �̵� �ӵ� ����
		// �޸��� ���� �� ���¹̳� ���� ����
		GetWorld()->GetTimerManager().ClearTimer(StaminaDecreaseStaminaForRunning);
	}
}

void AITTCharacter::Interactive()
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

void AITTCharacter::IncreaseMovementSpeed()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp && !bIsSpeedBoosted) // 속도 증가가 이미 적용된 상태가 아닌 경우에만 실행
	{
		MovementComp->MaxWalkSpeed = 1.5f;  // 이동 속도 증가
		MovementComp->JumpZVelocity = 1.5f; // 점프 속도 증가
		bIsSpeedBoosted = true;
	}
}
void AITTCharacter::ResetMovementSpeed()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp && bIsSpeedBoosted)
	{
		MovementComp->MaxWalkSpeed /= 1.5f;  // 이동 속도 원상 복귀
		MovementComp->JumpZVelocity /= 1.5f; // 점프 속도 원상 복귀
		bIsSpeedBoosted = false;
	}
}

void AITTCharacter::PlayHealingSound_Implementation()
{
	// ���⿡ �⺻ ���� �߰�
}

void AITTCharacter::PlayRespawnUI_Implementation()
{

}

//�÷��̾� ���� ó��
void AITTCharacter::Die()
{
	// ĳ������ ���� ó�� ����

	// ���� ��忡 ���� �˸���
	ADontForgetMeGameModeBase* GM = Cast<ADontForgetMeGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GM != nullptr)
	{
		PlayerHealthBar->RemoveFromParent();
		GM->PlayerDied(GetController());
		UE_LOG(LogTemp, Error, TEXT("Die"));
	}
}

