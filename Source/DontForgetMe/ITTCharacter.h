// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InterfaceInteractable.h"
#include "InterfaceGrip.h"
#include "CheckPoint.h"
#include "GameFramework/Character.h"
#include "ITTCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	Bear    UMETA(DisplayName = "Bear"),
	Robot   UMETA(DisplayName = "Robot")
};

UCLASS(config = Game)
class AITTCharacter : public ACharacter
{
	GENERATED_BODY()

	//	/** Camera boom positioning the camera behind the character */
	/*	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	///** Follow camera */
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;*/
public:
	AITTCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
		float TurnRateGamepad;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
		float MaxHealth = 5.0f;	// 최대 체력을 위한 변수

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
		float CurrentHealth; // 현재 체력

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
		float CurrentStamina;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
		float MaxStamina = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CheckPoint")
		FVector CheckPoint;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
		ECharacterType SelectedCharacterType;

	// UI 위젯 클래스의 참조
	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<UUserWidget> PlayerHealthBarClass;

	// UI 위젯 인스턴스
	UPROPERTY()
		UUserWidget* PlayerHealthBar;

	FTimerHandle SlowStateTimer; //슬로우 시간 타이머핸들

	UFUNCTION()
		// 체력 업데이트 함수
		void UpdateHealth();

	UFUNCTION()
		void UpdateStamina();


	// 스태미너 회복 관련 변수
	const float StaminaRecoveryRate = 0.2f; //5초에 1회복

	const float SlowStaminaRecoveryRate = 0.1f; //10초에 1회복

	bool bIsStaminaDepleted;

	// 스태미너 회복 함수
	UFUNCTION()
		void RecoverStamina(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
		void PlayHealingSound();
	virtual void PlayHealingSound_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void RestoreWalkSpeed(); // 이동 속도를 원래대로 복구하는 함수

	// 사망 처리 함수
	void Die();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Run")
	bool bIsRunning = false;

	void CheckJumpStamina();


protected:
	//===============변수===============
	FHitResult OutHit;

	FCollisionQueryParams QueryParams;

	IInterfaceInteractable* InteractableActor;

	AActor* AttachedActor;

	FCollisionShape LocalCollisionShape; //충돌범위

	ACharacter* MyCharacter;

	// 죽음 횟수를 추적하는 변수
	int32 DeathCount = 0;

	FVector TargetLocation; // 이동 목표 위치

	bool bIsStaminaRecovering;

	FTimerHandle StaminaDecreaseStaminaForRunning;

	FTimerHandle StaminaDecreaseStaminaForGrip;

	FTimerHandle StaminaDecreaseStaminaForThrow;

	FTimerHandle PossessCheckTimerHandle;

	FTimerHandle StaminaRecoveryTimer;

	const float SlowDuration = 10.0f; //슬로우 상태 시간

	//float StaminaAmount;


	//===============함수===============

	void CheckIfPossessed();

	//void SetStaminaRecovery(bool bSlowRecovery);

	void StartRunning();

	void StopRunning();

	//void AdjustMovementSpeed();

	void DecreaseStamina(float StaminaAmount);

	void DecreaseStaminaRunning();

	void RecoverStamina();

	//void CheckStaminaStatus();

	void CheckForController(); //컨트롤러 할당 확인 함수

	void CreateHealthBar(); //체력바 생성 함수

	void Interactive(); //상호작용

	bool CheckForObjectsInChannel(FVector StartLocation, FVector EndLocation, ECollisionChannel CollisionChannel, FCollisionShape CollisionShape);

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	FVector GetForwardLocation(float Distance); //정면 시점 계산

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;
	// End of APawn interface

public:
	///** Returns CameraBoom subobject **/
	/*FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	///** Returns FollowCamera subobject **/
	//FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	void IncreaseMovementSpeed();
	void ResetMovementSpeed();
};

