// Fill out your copyright notice in the Description page of Project Settings.

#include "Robot.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BearCharacter.h"



ARobot::ARobot()
{
	// 기본 이동 및 점프 속도 저장
	DefaultSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultJumpZVelocity = GetCharacterMovement()->JumpZVelocity;
	bIsEffectActive = false;

	// 자기장 시각적 표현을 위한 StaticMeshComponent 생성 및 설정
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
		//OtherCharacter->ResetMovementSpeed();

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

// E키 자기장
void ARobot::IncreaseSpeedAndJump()
{
	if (!bIsEffectActive)
	{
		bIsEffectActive = true;
		GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed * 1.5f;
		GetCharacterMovement()->JumpZVelocity = DefaultJumpZVelocity * 1.5f;
		MagneticField->SetVisibility(true);

		// 자기장을 1초 후에 비활성화
		GetWorld()->GetTimerManager().SetTimer(FieldVisibilityTimerHandle, this, &ARobot::HideMagneticField, 1.0f, false);

		// 타이머 설정하여 5초 후에 효과 리셋
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

		// 5초 후에 자동으로 NoCollision으로 설정하는 타이머 설정
		GetWorld()->GetTimerManager().SetTimer(MagneticFieldTimerHandle, this, &ARobot::DisableMagneticField, 1.0f, false);
	}
	else
	{
		// 즉시 비활성화, 타이머를 취소
		DisableMagneticField();
	}
}

void ARobot::DisableMagneticField()
{
	if (MagneticField->IsCollisionEnabled())
	{
		MagneticField->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 혹시 모르니 타이머를 취소
		GetWorld()->GetTimerManager().ClearTimer(MagneticFieldTimerHandle);
	}
}

void ARobot::CheckMagneticField()
{
	bIsMagneticFieldActive = !bIsMagneticFieldActive; // 자기장 활성화 상태 토글
}

bool ARobot::IsMagneticFieldActive() const
{
	return bIsMagneticFieldActive;
}