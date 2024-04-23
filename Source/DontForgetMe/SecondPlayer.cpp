// Copyright Epic Games, Inc. All Rights Reserved.

#include "SecondPlayer.h"
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
#include "ITTGameMode.h"


//////////////////////////////////////////////////////////////////////////
// ASecondPlayer

ASecondPlayer::ASecondPlayer()
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
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
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

	//ConnectionTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ConnectionTrigger"));
	//ConnectionTrigger->SetupAttachment(RootComponent);
	//ConnectionTrigger->SetBoxExtent(FVector(100, 100, 100));  // Adjust size as needed
	//ConnectionTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASecondPlayer::OnOverlapBegin);

	bIsGrabbingObject = false;
	ArmLength = 0.0f;
	MaxArmLength = 300.0f;
	ArmMovementSpeed = 100.0f;
	RaycastDistance = 100.0f;



	bAreObjectsConnected = false;


}

void ASecondPlayer::BeginPlay()
{
	Super::BeginPlay();

	//기본 체력 설정
	CurrentHealth = MaxHealth;

	// 기본 스태미너 설정

	CurrentStamina = MyMaxStamina;

	bIsStaminaDepleted = false;
	bIsRunning = false;

	CheckForController();

	LocalCollisionShape = FCollisionShape::MakeSphere(50.0f); //충돌 범위 50.0f 설정

	UE_LOG(LogTemp, Error, TEXT("RespawnTest"));

	//GetWorld()->GetTimerManager().SetTimer(StaminaRecoveryTimer, this, &ASecondPlayer::RecoverStamina, NormalRecoveryTime, true);

	GetWorld()->GetTimerManager().SetTimer(PossessCheckTimerHandle, this, &ASecondPlayer::CheckIfPossessed, 0.3f, false);

	UpdateHealth();
}

void ASecondPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DetectInteractableObjects();

	RecoverStamina(DeltaTime);

	UpdateHealth();
	UpdateStamina();

	//// 팔 길이를 조절하여 사물을 잡는 동작을 수행
	//if (bIsGrabbingObject)
	//{
	//	ExtendArm();
	//}
	//else
	//{
	//	RetractArm();
	//}

}


void ASecondPlayer::DetectInteractableObjects()
{
	FVector Location = GetActorLocation();
	FVector Start = Location;
	FVector End = Location + FVector(0, 0, 1); // Slightly above the player

	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(100.0f); // 1m = 100cm in Unreal

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	TArray<FHitResult> OutHits;

	if (GetWorld()->SweepMultiByChannel(OutHits, Start, End, FQuat::Identity, ECC_Visibility, MyColSphere, CollisionParams))
	{
		for (auto& Hit : OutHits)
		{
			AActor* Interactable = Hit.GetActor();
			if (Interactable)
			{
				// Here, you can do something with each interactable object
				// For example, log its name
				//UE_LOG(LogTemp, Warning, TEXT("Detected: %s"), *Interactable->GetName());
			}
		}
	}
}



//////////////////////////////////////////////////////////////////////////
// Input

void ASecondPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASecondPlayer::CheckJumpStamina);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ASecondPlayer::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ASecondPlayer::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ASecondPlayer::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ASecondPlayer::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASecondPlayer::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASecondPlayer::TouchStopped);

	//PlayerInputComponent->BindAction("Grip", IE_Pressed, this, &ASecondPlayer::GripAndDrop);

	//PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &ASecondPlayer::Throw);

	// 마우스 좌클릭 바인딩
	//PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &ASecondPlayer::GrabObject);


	//PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &ASecondPlayer::Interactive);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASecondPlayer::StartRunning);

	PlayerInputComponent->BindAction("Run", IE_Released, this, &ASecondPlayer::StopRunning);

	// E키 합치기
	//PlayerInputComponent->BindAction("Connect", IE_Pressed, this, &ASecondPlayer::ConnectNearbyObjects);


	MyCharacter = this;
}

void ASecondPlayer::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ASecondPlayer::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ASecondPlayer::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ASecondPlayer::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ASecondPlayer::MoveForward(float Value)
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

void ASecondPlayer::MoveRight(float Value)
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


FVector ASecondPlayer::GetForwardLocation(float Distance)
{
	FVector ForwardVector = GetActorForwardVector(); // 액터가 바라보는 방향
	ForwardVector *= Distance;
	ForwardVector += GetActorLocation();

	return ForwardVector;
}

bool ASecondPlayer::CheckForObjectsInChannel(FVector StartLocation, FVector EndLocation, ECollisionChannel CollisionChannel, FCollisionShape CollisionShape)
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

void ASecondPlayer::CheckForController()
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
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &ASecondPlayer::CheckForController, 0.01f, false);
	}
}

void ASecondPlayer::CheckIfPossessed()
{
	// 이 캐릭터가 아무 컨트롤러에도 소유되지 않았다면
	if (GetController() == nullptr)
	{
		// 캐릭터 삭제
		Destroy();
	}
}





void ASecondPlayer::CreateHealthBar()
{
	auto PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController != nullptr && PlayerHealthBarClass != nullptr)
	{
		PlayerHealthBar = CreateWidget<UUserWidget>(PlayerController, PlayerHealthBarClass);
		if (PlayerHealthBar)
		{
			PlayerHealthBar->AddToViewport(0);
			PlayerHealthBar->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Error, TEXT("%d"), PlayerController->PlayerState ? PlayerController->PlayerState->GetPlayerId() : -1);

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



//void ASecondPlayer::CreateHealthBar()
//{
//	auto PlayerController = Cast<APlayerController>(GetController());
//	// 현재 세션에서 첫 번째 로컬 플레이어 컨트롤러를 가져옵니다.
//	auto LocalPlayerController = GetWorld()->GetFirstPlayerController();
//
//	if (PlayerController != nullptr && PlayerHealthBarClass != nullptr)
//	{
//		PlayerHealthBar = CreateWidget<UUserWidget>(PlayerController, PlayerHealthBarClass);
//		if (PlayerHealthBar)
//		{
//			PlayerHealthBar->AddToViewport(0);
//			PlayerHealthBar->SetVisibility(ESlateVisibility::Visible);
//			UE_LOG(LogTemp, Error, TEXT("%d"), PlayerController->PlayerState ? PlayerController->PlayerState->GetPlayerId() : -1);
//			// 현재 PlayerController가 로컬 플레이어 컨트롤러와 같은지 확인합니다.
//			if (PlayerController == LocalPlayerController)
//			{
//				// 블루프린트 위젯의 VerticalBox 컴포넌트를 찾습니다.
//				UVerticalBox* MyVerticalBox = Cast<UVerticalBox>(PlayerHealthBar->GetWidgetFromName(TEXT("MyVerticalBox")));
//				if (MyVerticalBox)
//				{
//					FVector2D ViewportSize; //창의 사이즈
//					if (GEngine && GEngine->GameViewport)
//					{
//						GEngine->GameViewport->GetViewportSize(ViewportSize);
//
//						// 창의 너비는 절반을 사용합니다. 높이는 전체를 사용하지만, 하단에 약간의 여백을 두고 싶을 때는 여백을 빼줍니다.
//						float NewX = ViewportSize.X / 4; // 창이 세로로 나뉘었다고 가정할 때, 중앙을 의미합니다.
//						float NewY = ViewportSize.Y + (ViewportSize.Y / 1.5f);
//
//						FVector2D NewPosition(NewX, NewY);
//
//						// VerticalBox의 CanvasPanelSlot을 찾아 위치를 조정합니다.
//						UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MyVerticalBox->Slot);
//						if (CanvasSlot)
//						{
//							CanvasSlot->SetPosition(NewPosition);
//						}
//					}
//				}
//			}
//		}
//	}
//}

//void ASecondPlayer::GripAndDrop()
//{
//	if (!AttachedActor) // 잡고 있지 않다면
//	{
//		if (!bIsStaminaDepleted && CurrentStamina >= 1)
//		{
//			FVector StartLocation = GetForwardLocation(200.0f);
//			FVector EndLocation = GetForwardLocation(400.0f);
//
//			if (CheckForObjectsInChannel(StartLocation, EndLocation, ECC_GameTraceChannel1, LocalCollisionShape))
//			{
//				AttachedActor = OutHit.GetActor();
//				GrippableActor = Cast<IInterfaceGrip>(AttachedActor);
//
//				if (GrippableActor && AttachedActor)
//				{
//					DecreaseStamina(1.0f);
//					UE_LOG(LogTemp, Error, TEXT("DecreaseStamina"));
//					GrippableActor->Grip();
//					TargetLocation = GetActorLocation() + GetActorForwardVector() * 100.0f; // 현재 위치에서 전방으로 1미터 이동
//					GrippableActor->BeginMoveToTarget(this, TargetLocation);
//				}
//			}
//		}
//	}
//	else // 이미 잡고 있다면
//	{
//		GrippableActor = Cast<IInterfaceGrip>(AttachedActor);
//		if (GrippableActor)
//		{
//			GrippableActor->Drop();
//			GrippableActor->StopMoveToTarget();
//		}
//
//		AttachedActor->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
//		AttachedActor->SetActorLocation(GetActorLocation() + GetActorForwardVector() * 75.0f); // 현재 위치에서 전방으로 75.0f 이동
//
//		AttachedActor = nullptr;
//	}
//}
//
//void ASecondPlayer::GripAndDrop()
//{
//	if (!AttachedActor) // 잡고 있지 않다면
//	{
//		if (!bIsStaminaDepleted && CurrentStamina >= 1)
//		{
//			FVector StartLocation = GetForwardLocation(200.0f);
//			FVector EndLocation = GetForwardLocation(400.0f);
//
//			if (CheckForObjectsInChannel(StartLocation, EndLocation, ECC_GameTraceChannel1, LocalCollisionShape))
//			{
//				AttachedActor = OutHit.GetActor();
//				GrippableActor = Cast<IInterfaceGrip>(AttachedActor);
//
//				if (GrippableActor && AttachedActor)
//				{
//					DecreaseStamina(1.0f);
//					UE_LOG(LogTemp, Error, TEXT("DecreaseStamina"));
//					GrippableActor->Grip();
//					TargetLocation = GetForwardLocation(100.0f);
//					GrippableActor->BeginMoveToTarget(this, TargetLocation);
//				}
//			}
//		}
//	}
//	else // 이미 잡고 있다면
//	{
//		GrippableActor = Cast<IInterfaceGrip>(AttachedActor);
//		if (GrippableActor)
//		{
//			TargetLocation = GetForwardLocation(100.0f);
//			GrippableActor->BeginMoveToTarget(this, TargetLocation);
//		}
//	}
//}
//
//
//
//void ASecondPlayer::Throw()
//{
//	if (AttachedActor && CurrentStamina >= 2 && !bIsStaminaDepleted) // isGrip이 true이고, AttachedActor가 유효한지 확인
//	{
//		// 액터를 현재 위치에서 분리
//		AttachedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
//
//		// 루트 컴포넌트를 UPrimitiveComponent로 캐스팅
//		UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(AttachedActor->GetRootComponent());
//
//		if (RootPrimitiveComponent && RootPrimitiveComponent->IsSimulatingPhysics())
//		{
//			// 던지기 위한 초기 위치와 속도 설정
//			FVector LaunchVelocity = GetActorForwardVector() * 1000.0f; // 던지는 속도 및 방향 설정
//			RootPrimitiveComponent->AddImpulse(LaunchVelocity, NAME_None, true);
//
//		}
//
//		DecreaseStamina(2.0f);
//
//		GrippableActor->Drop(); // Drop 함수 호출
//		AttachedActor = nullptr;
//		GrippableActor = nullptr; // AttachedActor, GrippableActor 초기화
//	}
//}


//레이캐스트 코드

//void ASecondPlayer::GrabObject()
//{
//	if (bIsGrabbingObject) // 이미 사물을 잡고 있는 경우, 사물을 놓습니다.
//	{
//		ReleaseObject();
//		return;
//	}
//
//	FVector StartLocation = GetMesh()->GetSocketLocation("hand_socket");
//	FVector EndLocation = StartLocation + (GetActorForwardVector() * RaycastDistance);
//
//	FHitResult HitResult;
//	FCollisionQueryParams TraceParams(FName(TEXT("GrabTrace")), true, this);
//
//	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams))
//	{
//		AActor* HitActor = HitResult.GetActor();
//		if (HitActor)
//		{
//			if (GrabbedObject) // 이미 다른 객체를 잡고 있다면
//			{
//				ConnectObjects(GrabbedObject, HitActor); // 두 객체를 연결
//			}
//			GrabbedObject = HitActor; // 새로운 객체를 잡습니다.
//			bIsGrabbingObject = true;
//			UE_LOG(LogTemp, Log, TEXT("Grabbed object: %s"), *GrabbedObject->GetName());
//		}
//	}
//}
//
//void ASecondPlayer::CheckAndConnectNearbyObjects()
//{
//	if (!GrabbedObject)
//	{
//		return; // No object grabbed, nothing to do
//	}
//
//	FVector Location = GrabbedObject->GetActorLocation();
//	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(100.0f); // 1m = 100cm in Unreal
//
//	FCollisionQueryParams CollisionParams;
//	CollisionParams.AddIgnoredActor(this);
//	CollisionParams.AddIgnoredActor(GrabbedObject); // Ignore the currently grabbed object
//
//	TArray<FHitResult> OutHits;
//
//	if (GetWorld()->SweepMultiByChannel(OutHits, Location, Location + FVector(1, 0, 0), FQuat::Identity, ECC_Visibility, MyColSphere, CollisionParams))
//	{
//		for (auto& Hit : OutHits)
//		{
//			AActor* OtherActor = Hit.GetActor();
//			if (OtherActor && OtherActor != GrabbedObject)
//			{
//				// Auto-connect if within range
//				ConnectObjects(GrabbedObject, OtherActor);
//				break; // Optionally, stop after connecting to one object
//			}
//		}
//	}
//}
//
//void ASecondPlayer::ReleaseObject()
//{
//	if (GrabbedObject)
//	{
//		// 사물을 놓을 때 필요한 로직을 처리합니다. 예를 들어 사물을 분리할 수 있습니다.
//		GrabbedObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
//		GrabbedObject = nullptr;
//	}
//
//	bIsGrabbingObject = false; // 사물을 잡는 상태를 초기화합니다.
//	UE_LOG(LogTemp, Log, TEXT("object release."));
//
//}
//
//void ASecondPlayer::ExtendArm()
//{
//	ArmLength = FMath::FInterpTo(ArmLength, MaxArmLength, GetWorld()->GetDeltaSeconds(), ArmMovementSpeed);
//	if (GrabbedObject)
//	{
//		FVector NewArmEndLocation = GetMesh()->GetSocketLocation("hand_socket") + (GetActorForwardVector() * ArmLength);
//		GrabbedObject->SetActorLocation(NewArmEndLocation);
//	}
//}
//
//void ASecondPlayer::RetractArm()
//{
//	ArmLength = FMath::FInterpTo(ArmLength, 0.0f, GetWorld()->GetDeltaSeconds(), ArmMovementSpeed);
//
//	if (GrabbedObject)
//	{
//		FVector NewArmEndLocation = GetMesh()->GetSocketLocation("hand_socket") + (GetActorForwardVector() * ArmLength);
//		GrabbedObject->SetActorLocation(NewArmEndLocation);
//	}
//
//}





//
//void ASecondPlayer::Interactive()
//{
//	FVector StartLocation = GetForwardLocation(50.0f);
//	FVector EndLocation = GetForwardLocation(100.0f);
//	if (CheckForObjectsInChannel(StartLocation, EndLocation, ECC_GameTraceChannel2, LocalCollisionShape))
//	{
//		InteractableActor = Cast<IInteractable>(OutHit.GetActor());
//		if (InteractableActor)
//		{
//			InteractableActor->Interact(MyCharacter);
//		}
//	}
//}

void ASecondPlayer::UpdateHealth()
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

float ASecondPlayer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
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

void ASecondPlayer::Die()
{
	// 캐릭터의 죽음 처리 로직

	// 게임 모드에 죽음 알리기
	AITTGameMode* GM = Cast<AITTGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM != nullptr)
	{
		GM->PlayerDied(GetController());
		UE_LOG(LogTemp, Error, TEXT("Die"));
		Destroy();
	}
}


void ASecondPlayer::UpdateStamina()
{
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, MyMaxStamina);

	if (PlayerHealthBar)
	{
		UProgressBar* StaminaBar = Cast<UProgressBar>(PlayerHealthBar->GetWidgetFromName(TEXT("ProgressBar_stamina"))); // ProgressBar_Stamina는 스태미너 바의 이름입니다.
		if (StaminaBar)
		{
			StaminaBar->SetPercent(CurrentStamina / MyMaxStamina);

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

void ASecondPlayer::StartRunning()
{
	if (CurrentStamina > 0 && !bIsRunning && !bIsStaminaDepleted)
	{
		bIsRunning = true;
		GetCharacterMovement()->MaxWalkSpeed *= 2; // 이동 속도 증가
		// 달리기 시작할 때 스태미너 감소 시작
		GetWorld()->GetTimerManager().SetTimer(StaminaDecreaseStaminaForRunning, this, &ASecondPlayer::DecreaseStaminaRunning, 0.1f, true);
	}
}

void ASecondPlayer::StopRunning()
{
	if (bIsRunning)
	{
		bIsRunning = false;
		GetCharacterMovement()->MaxWalkSpeed /= 2; // 이동 속도 복원
		// 달리기 중지 시 스태미너 감소 중지
		GetWorld()->GetTimerManager().ClearTimer(StaminaDecreaseStaminaForRunning);
	}
}

void ASecondPlayer::DecreaseStamina(float StaminaAmount)
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
		GetWorld()->GetTimerManager().SetTimer(SlowStateTimer, this, &ASecondPlayer::RestoreWalkSpeed, SlowDuration, false);
	}
}

void ASecondPlayer::DecreaseStaminaRunning()
{
	DecreaseStamina(0.1f);
}

void ASecondPlayer::RecoverStamina(float DeltaTime)
{
	if (!bIsRunning && CurrentStamina < MyMaxStamina)
	{
		float RecoveryRate = bIsStaminaDepleted ? SlowStaminaRecoveryRate : StaminaRecoveryRate;
		CurrentStamina += RecoveryRate * DeltaTime;
		CurrentStamina = FMath::Min(CurrentStamina, MyMaxStamina);
	}
}

// 이동 속도를 원래대로 복구하는 함수
void ASecondPlayer::RestoreWalkSpeed()
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

void ASecondPlayer::CheckJumpStamina()
{
	if (CurrentStamina >= 1.0f && JumpIsAllowedInternal())
	{
		ACharacter::Jump();
		//DecreaseStamina(1.0f);
	}
}




//
////e키 합치기
//void ASecondPlayer::ConnectObjects(AActor* FirstObject, AActor* SecondObject)
//{
//	if (!FirstObject || !SecondObject)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Invalid objects for connection"));
//		return;
//	}
//
//	UPrimitiveComponent* FirstComponent = Cast<UPrimitiveComponent>(FirstObject->GetRootComponent());
//	UPrimitiveComponent* SecondComponent = Cast<UPrimitiveComponent>(SecondObject->GetRootComponent());
//
//	if (FirstComponent && SecondComponent)
//	{
//		// Use predefined attachment rules
//		FAttachmentTransformRules AttachRules = FAttachmentTransformRules::KeepWorldTransform;
//		SecondComponent->AttachToComponent(FirstComponent, AttachRules);
//
//		// Optionally log positions for debugging
//		FVector FirstPosition = FirstComponent->GetComponentLocation();
//		FVector SecondPosition = SecondComponent->GetComponentLocation();
//		UE_LOG(LogTemp, Log, TEXT("Objects connected: %s to %s at positions %s, %s"),
//			*FirstObject->GetName(), *SecondObject->GetName(),
//			*FirstPosition.ToString(), *SecondPosition.ToString());
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("No compatible components found for connection"));
//	}
//}
//
//void ASecondPlayer::ConnectNearbyObjects()
//{
//	if (!GrabbedObject)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("No grabbed object to connect or disconnect with others"));
//		return;
//	}
//
//	FVector Location = GrabbedObject->GetActorLocation();
//	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(100.0f);
//
//	FCollisionQueryParams CollisionParams;
//	CollisionParams.AddIgnoredActor(this);
//	CollisionParams.AddIgnoredActor(GrabbedObject);
//
//	TArray<FHitResult> OutHits;
//
//	if (GetWorld()->SweepMultiByChannel(OutHits, Location, Location + FVector(1, 0, 0), FQuat::Identity, ECC_Visibility, MyColSphere, CollisionParams))
//	{
//		for (auto& Hit : OutHits)
//		{
//			AActor* OtherActor = Hit.GetActor();
//			if (OtherActor && OtherActor != GrabbedObject)
//			{
//				if (bAreObjectsConnected)
//				{
//					// 분리 로직
//					DisconnectObjects(GrabbedObject, OtherActor);
//				}
//				else
//				{
//					// 연결 로직
//					ConnectObjects(GrabbedObject, OtherActor);
//				}
//			}
//		}
//	}
//
//	// 토글 상태 업데이트
//	bAreObjectsConnected = !bAreObjectsConnected;
//}
//
//
//void ASecondPlayer::DisconnectObjects(AActor* FirstObject, AActor* SecondObject)
//{
//	if (!FirstObject || !SecondObject)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Invalid objects for disconnection"));
//		return;
//	}
//
//	UPrimitiveComponent* FirstComponent = Cast<UPrimitiveComponent>(FirstObject->GetRootComponent());
//	UPrimitiveComponent* SecondComponent = Cast<UPrimitiveComponent>(SecondObject->GetRootComponent());
//
//	if (FirstComponent && SecondComponent)
//	{
//		SecondComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
//		UE_LOG(LogTemp, Log, TEXT("Objects disconnected: %s from %s"),
//			*FirstObject->GetName(), *SecondObject->GetName());
//	}
//	else
//	{

//		UE_LOG(LogTemp, Warning, TEXT("No compatible components found for disconnection"));
//	}
//}
//
//void ASecondPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor && OtherActor != this)
//	{
//		ConnectObjects(this, OtherActor);
//	}
//}
