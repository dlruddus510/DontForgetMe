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
		float MaxHealth = 5.0f;	// �ִ� ü���� ���� ����

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
		float CurrentHealth; // ���� ü��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
		float CurrentStamina;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
		float MyMaxStamina = 10.0f;

	// UI ���� Ŭ������ ����
	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<UUserWidget> PlayerHealthBarClass;

	// UI ���� �ν��Ͻ�
	UPROPERTY()
		UUserWidget* PlayerHealthBar;

	FTimerHandle SlowStateTimer; //���ο� �ð� Ÿ�̸��ڵ�

	UFUNCTION()
		// ü�� ������Ʈ �Լ�
		void UpdateHealth();

	UFUNCTION()
		void UpdateStamina();

	// ��� ó�� �Լ�
	//void Die();


	// ���¹̳� ȸ�� ���� ����
	const float StaminaRecoveryRate = 0.2f; //5�ʿ� 1ȸ��

	const float SlowStaminaRecoveryRate = 0.1f; //10�ʿ� 1ȸ��

	bool bIsStaminaDepleted;

	// ���¹̳� ȸ�� �Լ�
	UFUNCTION()
		void RecoverStamina(float DeltaTime);


	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void RestoreWalkSpeed(); // �̵� �ӵ��� ������� �����ϴ� �Լ�
protected:
	//===============����===============
	FHitResult OutHit;

	FCollisionQueryParams QueryParams;
	
	IInterfaceInteractable* InteractableActor;

	AActor* AttachedActor;

	FCollisionShape LocalCollisionShape; //�浹����

	ACharacter* MyCharacter;

	// ���� Ƚ���� �����ϴ� ����
	int32 DeathCount = 0;

	FVector TargetLocation; // �̵� ��ǥ ��ġ

	bool bIsStaminaRecovering;

	FTimerHandle StaminaDecreaseStaminaForRunning;

	FTimerHandle StaminaDecreaseStaminaForGrip;

	FTimerHandle StaminaDecreaseStaminaForThrow;

	FTimerHandle PossessCheckTimerHandle;

	FTimerHandle StaminaRecoveryTimer;

	const float SlowDuration = 10.0f; //���ο� ���� �ð�

	//float StaminaAmount;
	bool bIsSpeedBoosted;


	//===============�Լ�===============

	void CheckIfPossessed();

	//void SetStaminaRecovery(bool bSlowRecovery);

	void StartRunning();

	void StopRunning();

	//void AdjustMovementSpeed();

	void DecreaseStamina(float StaminaAmount);

	void DecreaseStaminaRunning();

	void RecoverStamina();

	//void CheckStaminaStatus();

	void CheckForController(); //��Ʈ�ѷ� �Ҵ� Ȯ�� �Լ�

	void CreateHealthBar(); //ü�¹� ���� �Լ�

	void Interactive(); //��ȣ�ۿ�

	bool CheckForObjectsInChannel(FVector StartLocation, FVector EndLocation, ECollisionChannel CollisionChannel, FCollisionShape CollisionShape);

	FVector GetForwardLocation(float Distance); //���� ���� ���

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

	// �ڱ��忡 ĳ���Ͱ� �������� �� ȣ��Ǵ� �Լ�
	UFUNCTION()
		void OnFieldEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// �ڱ��忡�� ĳ���Ͱ� ����� �� ȣ��Ǵ� �Լ�
	UFUNCTION()
		void OnFieldExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FTimerHandle SpeedResetTimerHandle;  // �̵� �ӵ� �� ������ ���� ���¸� ������� �����ϱ� ���� Ÿ�̸� �ڵ�

public:

	void CheckMagneticField();
	// �ڱ��� Ȱ��ȭ ���¸� ��ȯ
	bool IsMagneticFieldActive() const;
};

