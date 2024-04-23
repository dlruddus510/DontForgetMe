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

}

void AITTCharacter::BeginPlay()
{
	Super::BeginPlay();

	//기본 체력 설정
	CurrentHealth = MaxHealth;

	// 기본 스태미너 설정

	CurrentStamina = MaxStamina;

	bIsStaminaDepleted = false;
	bIsRunning = false;

	CheckForController();

	LocalCollisionShape = FCollisionShape::MakeSphere(80.0f); //충돌 범위 50.0f 설정

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

//플레이어 컨트롤러 및 Possess 체크
void AITTCharacter::CheckForController()
{
	if (GetController() != nullptr)
	{
		// 컨트롤러가 있을 경우 위젯 생성 로직 실행
		CreateHealthBar();
	}
	else
	{
		// 컨트롤러가 없을 경우 0.01초 후 다시 확인
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AITTCharacter::CheckForController, 0.01f, false);
	}
}

void AITTCharacter::CheckIfPossessed()
{
	// 이 캐릭터가 아무 컨트롤러에도 소유되지 않았다면
	if (GetController() == nullptr)
	{
		// 캐릭터 삭제
		Destroy();
	}
}

//플레이어 충돌 범위 체크
FVector AITTCharacter::GetForwardLocation(float Distance)
{
	FVector ForwardVector = GetActorForwardVector(); // 액터가 바라보는 방향
	ForwardVector *= Distance;
	ForwardVector += GetActorLocation();

	return ForwardVector;
}

bool AITTCharacter::CheckForObjectsInChannel(FVector StartLocation, FVector EndLocation, ECollisionChannel CollisionChannel, FCollisionShape CollisionShape)
{
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(this); // 자기 자신은 충돌에서 무시

	// SweepSingleByChannel을 사용하여 지정된 범위와 채널에서 오브젝트가 있는지 확인
	bool bIsObjectFound = GetWorld()->SweepSingleByChannel(
		OutHit,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		CollisionChannel,
		CollisionShape,
		QueryParams
	);

	return bIsObjectFound && OutHit.bBlockingHit; // 충돌이 발생했는지 여부를 반환
}

void AITTCharacter::CreateHealthBar()
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

			// 모든 플레이어 컨트롤러를 순회하여 가장 낮은 PlayerId 찾기
			int32 LowestPlayerId = INT32_MAX; // 가장 큰 int 값으로 초기화
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

			// 현재 PlayerController의 PlayerId
			int32 CurrentPlayerId = PlayerController->PlayerState ? PlayerController->PlayerState->GetPlayerId() : -1;

			// UI 위치 설정
			float NewX, NewY;
			FVector2D ViewportSize;
			if (GEngine && GEngine->GameViewport)
			{
				GEngine->GameViewport->GetViewportSize(ViewportSize);
				// 기존 플레이어의 경우
				if (CurrentPlayerId == LowestPlayerId)
				{
					NewX = (-(ViewportSize.X / 2.0f)) - 250;
					UE_LOG(LogTemp, Error, TEXT("NewX : %f"), ViewportSize.X);
					NewY = ViewportSize.Y - (ViewportSize.Y / 5.f);
				}
				else // 나중에 추가된 플레이어의 경우
				{
					NewX = (ViewportSize.X / 2.0f) - 250; // 화면의 오른쪽 중간
					NewY = ViewportSize.Y - (ViewportSize.Y / 5.f);
				}

				FVector2D NewPosition(NewX, NewY);

				// VerticalBox의 CanvasPanelSlot을 찾아 위치를 조정합니다.
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

//체력 및 스태미나 관리
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
		UProgressBar* StaminaBar = Cast<UProgressBar>(PlayerHealthBar->GetWidgetFromName(TEXT("ProgressBar_stamina"))); // ProgressBar_Stamina는 스태미너 바의 이름입니다.
		if (StaminaBar)
		{
			StaminaBar->SetPercent(CurrentStamina / MaxStamina);

			if (CurrentStamina <= 1.0f)
			{
				FLinearColor NewColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // RGB 색상 코드 + 알파(투명도) 값
				StaminaBar->SetFillColorAndOpacity(NewColor);
			}
			if (CurrentStamina > 1.0f)
			{
				FLinearColor NewColor = FLinearColor(0.5f, 1.0f, 0.27f, 1.0f); // RGB 색상 코드 + 알파(투명도) 값
				StaminaBar->SetFillColorAndOpacity(NewColor);
			}
		}
	}
}

void AITTCharacter::DecreaseStamina(float StaminaAmount)
{

	CurrentStamina -= StaminaAmount; // 예: 달리기로 인한 스태미너 감소
	if (CurrentStamina <= 0.2)
	{
		CurrentStamina = 0;
		bIsStaminaDepleted = true;
		StopRunning(); // 스태미너가 다 소진되면 달리기 중지
		// 슬로우 상태로 전환
		if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
		{
			CharMovement->MaxWalkSpeed /= 2; // 이동 속도를 절반으로 줄임
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
			CharMovement->MaxWalkSpeed *= 2; // 이동 속도를 원래대로 복구
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
	//	Die();
	}
	return ActualDamage;
}

//달리기 및 상호작용

void AITTCharacter::StartRunning()
{
	if (CurrentStamina > 0 && !bIsRunning && !bIsStaminaDepleted)
	{
		bIsRunning = true;
		GetCharacterMovement()->MaxWalkSpeed = 600.0f; // 이동 속도 증가
		// 달리기 시작할 때 스태미너 감소 시작
		GetWorld()->GetTimerManager().SetTimer(StaminaDecreaseStaminaForRunning, this, &AITTCharacter::DecreaseStaminaRunning, 0.1f, true);
	}
}

void AITTCharacter::StopRunning()
{
	if (bIsRunning)
	{
		bIsRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = 250.0f; // 이동 속도 복원
		// 달리기 중지 시 스태미너 감소 중지
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

//플레이어 죽음 처리
//void AITTCharacter::Die()
//{
//	// 캐릭터의 죽음 처리 로직
//
//	// 게임 모드에 죽음 알리기
//	ADontForgetMeGameModeBase* GM = Cast<AITTGameMode>(UGameplayStatics::GetGameMode(this));
//	if (GM != nullptr)
//	{
//		GM->PlayerDied(GetController());
//		UE_LOG(LogTemp, Error, TEXT("Die"));
//		Destroy();
//	}
//}

