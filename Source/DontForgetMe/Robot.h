
#pragma once

#include "CoreMinimal.h"
#include "ITTCharacter.h"
#include "Robot.generated.h"


UCLASS()
class DONTFORGETME_API ARobot : public AITTCharacter
{
	GENERATED_BODY()

	ARobot();

	virtual void BeginPlay() override;

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	


	void IncreaseSpeedAndJump();
	void NormalizeSpeedAndJump();
	void ResetEffect();
	void HideMagneticField();


	void ToggleMagneticField();
	void DisableMagneticField();


	bool bIsEffectActive;
	float DefaultSpeed;
	float DefaultJumpZVelocity;

	FTimerHandle FieldVisibilityTimerHandle;
	FTimerHandle EffectTimerHandle;
	FTimerHandle MagneticFieldTimerHandle;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MagneticField;
	FTimerHandle PossessCheckTimerHandle;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Magnetic Field")
	bool bIsInMagneticField;

	UPROPERTY(VisibleAnywhere, Category = "Magnetic Field")
	bool bIsMagneticFieldActive;

	UFUNCTION()
	void OnFieldEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION()
	void OnFieldExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void CheckMagneticField();

	bool IsMagneticFieldActive() const;
};
