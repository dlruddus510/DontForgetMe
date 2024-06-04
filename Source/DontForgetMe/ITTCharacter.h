
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

public:
	AITTCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
		float TurnRateGamepad;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
		float MaxHealth = 5.0f;	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
		float CurrentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
		float CurrentStamina;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
		float MaxStamina = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CheckPoint")
		FVector CheckPoint;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character")
		ECharacterType SelectedCharacterType;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<UUserWidget> PlayerHealthBarClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<UUserWidget> PlayerRespawnClass;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
		UUserWidget* PlayerHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
		UUserWidget* PlayerRespawn;

	FTimerHandle SlowStateTimer; 

	UFUNCTION()

		void UpdateHealth();

	UFUNCTION()
		void UpdateStamina();



	const float StaminaRecoveryRate = 0.2f; 

	const float SlowStaminaRecoveryRate = 0.1f; 

	bool bIsStaminaDepleted;

	bool bIsSpeedBoosted = false;


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

	void RestoreWalkSpeed();


	void Die();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Run")
	bool bIsRunning = false;

	void CheckJumpStamina();


protected:

	FHitResult OutHit;

	FCollisionQueryParams QueryParams;

	IInterfaceInteractable* InteractableActor;

	AActor* AttachedActor;

	FCollisionShape LocalCollisionShape;

	ACharacter* MyCharacter;


	int32 DeathCount = 0;

	FVector TargetLocation; 

	bool bIsStaminaRecovering;

	FTimerHandle StaminaDecreaseStaminaForRunning;

	FTimerHandle StaminaDecreaseStaminaForGrip;

	FTimerHandle StaminaDecreaseStaminaForThrow;

	FTimerHandle PossessCheckTimerHandle;

	FTimerHandle StaminaRecoveryTimer;

	const float SlowDuration = 10.0f; 



	void CheckIfPossessed();


	void StartRunning();

	void StopRunning();


	void DecreaseStamina(float StaminaAmount);

	void DecreaseStaminaRunning();

	void RecoverStamina();


	void CheckForController();

	void CreateHealthBar(); 

	void Interactive(); 

	bool CheckForObjectsInChannel(FVector StartLocation, FVector EndLocation, ECollisionChannel CollisionChannel, FCollisionShape CollisionShape);


	void MoveForward(float Value);


	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	FVector GetForwardLocation(float Distance); 

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;


public:
	
	void IncreaseMovementSpeed();
	void ResetMovementSpeed();
};

