// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "InterfaceGrip.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "SecondPlayer.generated.h"

UCLASS(config = Game)
class ASecondPlayer : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ASecondPlayer();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	float TurnRateGamepad;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RobotHealth")
	float MaxHealth = 5.0f;	// 최대 체력을 위한 변수

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RobotHealth")
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


	// 스태미너 회복 관련 변수
	const float StaminaRecoveryRate = 0.2f; //5초에 1회복

	const float SlowStaminaRecoveryRate = 0.1f; //10초에 1회복

	bool bIsStaminaDepleted;

	// 스태미너 회복 함수
	UFUNCTION()
	void RecoverStamina(float DeltaTime);


	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void RestoreWalkSpeed(); // 이동 속도를 원래대로 복구하는 함수

	// 사망 처리 함수
	void Die();

	bool bIsRunning = false;

	void CheckJumpStamina();

protected:
	//===============변수===============
	FHitResult OutHit;

	FCollisionQueryParams QueryParams;

	IInteractable* InteractableActor;

	IInterfaceGrip* GrippableActor;

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

	FTimerHandle StaminaRecoveryTimer;

	FTimerHandle PossessCheckTimerHandle;

	const float SlowDuration = 10.0f; //슬로우 상태 시간

	//float StaminaAmount;


	//===============함수===============

	void CheckIfPossessed();

	void SetStaminaRecovery(bool bSlowRecovery);

	void StartRunning();

	void StopRunning();

	void AdjustMovementSpeed();

	void DecreaseStamina(float StaminaAmount);

	void DecreaseStaminaRunning();

	void RecoverStamina();

	void CheckStaminaStatus();

	void CheckForController(); //컨트롤러 할당 확인 함수

	void CreateHealthBar(); //체력바 생성 함수

	void GripAndDrop(); //들기&놓기

	void Throw(); //던지기

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
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


public:

	void DetectInteractableObjects();

	// E키 합치기
	void ConnectObjects(AActor* FirstObject, AActor* SecondObject);
	void CheckAndConnectNearbyObjects();
	void ConnectNearbyObjects();
	void DisconnectObjects(AActor* FirstObject, AActor* SecondObject);



	AActor* GrabbedObject;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* ConnectionTrigger;

	//UFUNCTION()
	//void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// 잡는 동작
	void GrabObject();

	// 팔 늘리고 줄이고
	void ExtendArm();
	void RetractArm();
	void ReleaseObject();

	//사물을 현재 잡고있는지의 경우의 수
	bool bIsGrabbingObject;

	// 팔 현재 길이
	float ArmLength;
	bool bAreObjectsConnected;

	// 팔 길이
	UPROPERTY(EditAnywhere, Category = "Grabbing")
	float MaxArmLength;
	// 팔 속도
	UPROPERTY(EditAnywhere, Category = "Grabbing")
	float ArmMovementSpeed;

	UPROPERTY(EditAnywhere, Category = "Grabbing")
	float RaycastDistance;

};

