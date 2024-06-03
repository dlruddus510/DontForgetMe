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
		float MaxHealth = 5.0f;	// �ִ� ü���� ���� ����

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
		float CurrentHealth; // ���� ü��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
		float CurrentStamina;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
		float MaxStamina = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CheckPoint")
		FVector CheckPoint;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
		ECharacterType SelectedCharacterType;

	// UI ���� Ŭ������ ����
	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<UUserWidget> PlayerHealthBarClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<UUserWidget> PlayerRespawnClass;

	// UI ���� �ν��Ͻ�
	UPROPERTY()
		UUserWidget* PlayerHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
		UUserWidget* PlayerRespawn;

	FTimerHandle SlowStateTimer; //���ο� �ð� Ÿ�̸��ڵ�

	UFUNCTION()
		// ü�� ������Ʈ �Լ�
		void UpdateHealth();

	UFUNCTION()
		void UpdateStamina();


	// ���¹̳� ȸ�� ���� ����
	const float StaminaRecoveryRate = 0.2f; //5�ʿ� 1ȸ��

	const float SlowStaminaRecoveryRate = 0.1f; //10�ʿ� 1ȸ��

	bool bIsStaminaDepleted;

	bool bIsSpeedBoosted = false;

	// ���¹̳� ȸ�� �Լ�
	UFUNCTION()
		void RecoverStamina(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
		void PlayHealingSound();
	virtual void PlayHealingSound_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Respawn")
		void PlayRespawnUI();
	virtual void PlayRespawnUI_Implementation();

	void RestoreWalkSpeed(); // �̵� �ӵ��� ������� �����ϴ� �Լ�

	// ��� ó�� �Լ�
	void Die();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Run")
	bool bIsRunning = false;

	void CheckJumpStamina();


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

	FVector GetForwardLocation(float Distance); //���� ���� ���

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

