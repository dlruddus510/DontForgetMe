// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InterfaceInteractable.h"
#include "InterfaceGrip.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "Robot.generated.h"

UCLASS(config = Game)
class ARobot : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	ARobot();

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
		float MyMaxStamina = 10.0f;

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

	// 사망 처리 함수
	//void Die();


	// 스태미너 회복 관련 변수
	const float StaminaRecoveryRate = 0.2f; //5초에 1회복

	const float SlowStaminaRecoveryRate = 0.1f; //10초에 1회복

	bool bIsStaminaDepleted;

	// 스태미너 회복 함수
	UFUNCTION()
		void RecoverStamina(float DeltaTime);


	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void RestoreWalkSpeed(); // 이동 속도를 원래대로 복구하는 함수
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
	bool bIsSpeedBoosted;


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

	FVector GetForwardLocation(float Distance); //정면 시점 계산

	void IncreaseMovementSpeed();

	void ResetMovementSpeed();

protected:

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

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
public:

	bool bIsRunning = false;

public:
	void IncreaseSpeedAndJump();
	void NormalizeSpeedAndJump();
	void ResetEffect();
	void HideMagneticField();
	void ToggleMagneticField();
	void DisableMagneticField();

protected:
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* MagneticField;

private:
	float DefaultSpeed;
	float DefaultJumpZVelocity;
	bool bIsEffectActive;

	FTimerHandle FieldVisibilityTimerHandle;
	FTimerHandle EffectTimerHandle;
	FTimerHandle MagneticFieldTimerHandle;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Magnetic Field")
		bool bIsInMagneticField;

	UPROPERTY(VisibleAnywhere, Category = "Magnetic Field")
		bool bIsMagneticFieldActive;

	// 자기장에 캐릭터가 진입했을 때 호출되는 함수
	UFUNCTION()
		void OnFieldEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 자기장에서 캐릭터가 벗어났을 때 호출되는 함수
	UFUNCTION()
		void OnFieldExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FTimerHandle SpeedResetTimerHandle;  // 이동 속도 및 점프력 증가 상태를 원래대로 복구하기 위한 타이머 핸들

public:

	void CheckMagneticField();
	// 자기장 활성화 상태를 반환
	bool IsMagneticFieldActive() const;
};

