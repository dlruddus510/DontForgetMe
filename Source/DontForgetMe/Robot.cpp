
#include "Robot.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BearCharacter.h"



ARobot::ARobot()
{

	DefaultSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultJumpZVelocity = GetCharacterMovement()->JumpZVelocity;
	bIsEffectActive = false;

	
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

	GetWorld()->GetTimerManager().SetTimer(PossessCheckTimerHandle, this, &ARobot::CheckIfPossessed, 0.1f, false);

	MagneticField->OnComponentBeginOverlap.AddDynamic(this, &ARobot::OnFieldEnter);
	MagneticField->OnComponentEndOverlap.AddDynamic(this, &ARobot::OnFieldExit);

}

void ARobot::OnFieldEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABearCharacter* OtherCharacter = Cast<ABearCharacter>(OtherActor);
	if (OtherCharacter)
	{
		OtherCharacter->IncreaseMovementSpeed();
	}
}

void ARobot::OnFieldExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABearCharacter* OtherCharacter = Cast<ABearCharacter>(OtherActor);
	if (OtherCharacter)
	{


		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, OtherCharacter, &ABearCharacter::ResetMovementSpeed, 5.0f, false);
	}
}

void ARobot::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("IncreaseEffect", IE_Pressed, this, &ARobot::IncreaseSpeedAndJump);
	PlayerInputComponent->BindAction("ToggleMagneticField", IE_Pressed, this, &ARobot::ToggleMagneticField);
}


void ARobot::IncreaseSpeedAndJump()
{
	if (!bIsEffectActive)
	{
		bIsEffectActive = true;
		GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed * 1.5f;
		GetCharacterMovement()->JumpZVelocity = DefaultJumpZVelocity * 1.5f;
		MagneticField->SetVisibility(true);

	
		GetWorld()->GetTimerManager().SetTimer(FieldVisibilityTimerHandle, this, &ARobot::HideMagneticField, 1.0f, false);

	
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

	
		GetWorld()->GetTimerManager().SetTimer(MagneticFieldTimerHandle, this, &ARobot::DisableMagneticField, 1.0f, false);
	}
	else
	{

		DisableMagneticField();
	}
}

void ARobot::DisableMagneticField()
{
	if (MagneticField->IsCollisionEnabled())
	{
		MagneticField->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		GetWorld()->GetTimerManager().ClearTimer(MagneticFieldTimerHandle);
	}
}

void ARobot::CheckMagneticField()
{
	bIsMagneticFieldActive = !bIsMagneticFieldActive; 
}

bool ARobot::IsMagneticFieldActive() const
{
	return bIsMagneticFieldActive;
}