
#include "ITTCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Engine/UserInterfaceSettings.h"
#include "TimerManager.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DontForgetMeGameModeBase.h"




AITTCharacter::AITTCharacter()
{
	PrimaryActorTick.bCanEverTick = true;


	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	TurnRateGamepad = 50.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 250.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	
}

void AITTCharacter::BeginPlay()
{
	Super::BeginPlay();



	CurrentHealth = MaxHealth;



	CurrentStamina = MaxStamina;

	bIsStaminaDepleted = false;
	bIsRunning = false;

	CheckForController();

	LocalCollisionShape = FCollisionShape::MakeSphere(80.0f);

	UE_LOG(LogTemp, Error, TEXT("RespawnTest"));


	GetWorld()->GetTimerManager().SetTimer(PossessCheckTimerHandle, this, &AITTCharacter::CheckIfPossessed, 0.3f, false);

	UpdateHealth();
}

void AITTCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RecoverStamina(DeltaTime);

	UpdateHealth();
	UpdateStamina();

	//if (bSpawnParticlesOnlyWhenWalking)
	//{
	//	if (GetVelocity().Size() > 0) // Character is moving
	//	{
	//		SpawnFootstepParticles();
	//	}
	//}
	//else
	//{
	//	SpawnFootstepParticles();
	//}
}


void AITTCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AITTCharacter::CheckJumpStamina);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AITTCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AITTCharacter::MoveRight);


	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AITTCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AITTCharacter::LookUpAtRate);


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

	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AITTCharacter::LookUpAtRate(float Rate)
{

	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AITTCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);


		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AITTCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);


		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
	}
}


void AITTCharacter::CheckForController()
{
	if (GetController() != nullptr)
	{

		CreateHealthBar();
	}
	else
	{

		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AITTCharacter::CheckForController, 0.01f, false);
	}
}

void AITTCharacter::CheckIfPossessed()
{

	if (GetController() == nullptr)
	{

		Destroy();
	}
}

FVector AITTCharacter::GetForwardLocation(float Distance)
{
	FVector ForwardVector = GetActorForwardVector();
	ForwardVector *= Distance;
	ForwardVector += GetActorLocation();

	return ForwardVector;
}

bool AITTCharacter::CheckForObjectsInChannel(FVector StartLocation, FVector EndLocation, ECollisionChannel CollisionChannel, FCollisionShape CollisionShape)
{
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(this); 


	bool bIsObjectFound = GetWorld()->SweepSingleByChannel(
		OutHit,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		CollisionChannel,
		CollisionShape,
		QueryParams
	);

	return bIsObjectFound && OutHit.bBlockingHit;
}

void AITTCharacter::CreateHealthBar()
{
	auto PlayerController = Cast<APlayerController>(GetController());
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);

	if (PlayerController != nullptr && PlayerHealthBarClass != nullptr)
	{
		PlayerHealthBar = CreateWidget<UUserWidget>(PlayerController, PlayerHealthBarClass);
		PlayerRespawn = CreateWidget<UUserWidget>(PlayerController, PlayerRespawnClass);
		if (PlayerHealthBar)
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerHealthBar Suc"));
			PlayerHealthBar->AddToViewport(-1);
			PlayerHealthBar->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Error, TEXT("Player ID: %d"), PlayerController->PlayerState ? PlayerController->PlayerState->GetPlayerId() : -1);

	
			int32 LowestPlayerId = INT32_MAX; 
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


			int32 CurrentPlayerId = PlayerController->PlayerState ? PlayerController->PlayerState->GetPlayerId() : -1;


			float NewX, NewY;
			float ViewportScale = 1.333333f;
			FVector2D ViewportSize;
			if (GEngine && GEngine->GameViewport)
			{
				GEngine->GameViewport->GetViewportSize(ViewportSize);

				ViewportScale = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));

				ViewportSize.X /= ViewportScale;
				ViewportSize.Y /= ViewportScale;

				float UIWidth = 500.0f;
				float UIHeight = 56.0f;

				if (CurrentPlayerId == LowestPlayerId)
				{
					NewX = ((ViewportSize.X / 4.0f) - (UIWidth / 2.0f));
					NewY = (ViewportSize.Y * 0.8f) - (UIHeight * 0.5f);
				}
				else 
				{
					NewX = ((3.0f * ViewportSize.X / 4.0f) - (UIWidth / 2.0f));
					NewY = (ViewportSize.Y * 0.8f) - (UIHeight * 0.5f);
				}

				FVector2D NewPosition(NewX, NewY);

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
		UProgressBar* StaminaBar = Cast<UProgressBar>(PlayerHealthBar->GetWidgetFromName(TEXT("ProgressBar_stamina"))); 
		if (StaminaBar)
		{
			StaminaBar->SetPercent(CurrentStamina / MaxStamina);

			if (CurrentStamina <= 1.0f)
			{
				FLinearColor NewColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); 
				StaminaBar->SetFillColorAndOpacity(NewColor);
			}
			if (CurrentStamina > 1.0f)
			{
				FLinearColor NewColor = FLinearColor(0.5f, 1.0f, 0.27f, 1.0f); 
				StaminaBar->SetFillColorAndOpacity(NewColor);
			}
		}
	}
}

void AITTCharacter::DecreaseStamina(float StaminaAmount)
{

	CurrentStamina -= StaminaAmount; 
	if (CurrentStamina <= 0.2)
	{
		CurrentStamina = 0;
		bIsStaminaDepleted = true;
		StopRunning(); 
		if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
		{
			CharMovement->MaxWalkSpeed /= 2;
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
			CharMovement->MaxWalkSpeed *= 2; 
			bIsStaminaDepleted = false;
		}
	}
}

void AITTCharacter::CheckJumpStamina()
{
	if (CurrentStamina >= 1.0f && JumpIsAllowedInternal())
	{
		ACharacter::Jump();
	}
}

void AITTCharacter::ShowDamageWidget()
{
	if (DamageWidgetClass)
	{
		UUserWidget* DamageWidget = CreateWidget<UUserWidget>(GetWorld(), DamageWidgetClass);
		if (DamageWidget)
		{
			DamageWidget->AddToViewport();

			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AITTCharacter::HideDamageWidget, 0.2f, false);

			ActiveDamageWidget = DamageWidget; // Keep reference to widget for later removal
		}
	}
}

void AITTCharacter::HideDamageWidget()
{
	if (ActiveDamageWidget)
	{
		ActiveDamageWidget->RemoveFromViewport();
		ActiveDamageWidget = nullptr;
	}
}

float AITTCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageCauser == nullptr)
	{
		return 0.0f;
	}

	if (EventInstigator == nullptr)
	{
		return 0.0f;
	}
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CurrentHealth -= ActualDamage;

	UpdateHealth();
	ShowDamageWidget();

	if (CurrentHealth <= 0.f)
	{
		Die();
	}
	return ActualDamage;
}

void AITTCharacter::StartRunning()
{
	if (CurrentStamina > 0 && !bIsRunning && !bIsStaminaDepleted)
	{
		bIsRunning = true;
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;

		GetWorld()->GetTimerManager().SetTimer(StaminaDecreaseStaminaForRunning, this, &AITTCharacter::DecreaseStaminaRunning, 0.1f, true);
	}
}

void AITTCharacter::StopRunning()
{
	if (bIsRunning)
	{
		bIsRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = 250.0f;
	
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
	if (MovementComp && !bIsSpeedBoosted)
	{
		MovementComp->MaxWalkSpeed *= 1.5f; 
		MovementComp->JumpZVelocity *= 1.5f;
		bIsSpeedBoosted = true;
	}
}
void AITTCharacter::ResetMovementSpeed()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp && bIsSpeedBoosted)
	{
		MovementComp->MaxWalkSpeed /= 1.5f;  
		MovementComp->JumpZVelocity /= 1.5f; 
		bIsSpeedBoosted = false;
	}
}

void AITTCharacter::PlayHealingSound_Implementation()
{

}

void AITTCharacter::PlayRespawnUI_Implementation()
{

}


void AITTCharacter::Die()
{

	ADontForgetMeGameModeBase* GM = Cast<ADontForgetMeGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GM != nullptr)
	{
		PlayerHealthBar->RemoveFromParent();
		GM->PlayerDied(GetController());
		UE_LOG(LogTemp, Error, TEXT("Die"));
	}
}

void AITTCharacter::SpawnPickupParticles()
{
	if (NiagaraSystem)
	{
		
		FVector FootLocation = GetActorLocation();
		FRotator Rotation = GetActorRotation();
		FVector Scale = FVector(2.0f);
		UWorld* World = GetWorld();
		if (World)
		{
		
			UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
				NiagaraSystem,
				GetRootComponent(), 
				NAME_None,
				FVector::ZeroVector, 
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true,
				true 
			);

			if (NiagaraComponent)
			{
			
				NiagaraComponent->SetWorldScale3D(Scale);

				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([NiagaraComponent]()
					{
						if (NiagaraComponent)
						{
							NiagaraComponent->DestroyComponent();
						}
					}), 3.5f, false);
			}
		}
	}
}

void AITTCharacter::SpawnFootstepParticles()
{

}