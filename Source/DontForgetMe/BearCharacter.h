

#pragma once

#include "CoreMinimal.h"
#include "ITTCharacter.h"
#include "BearCharacter.generated.h"


UCLASS()
class DONTFORGETME_API ABearCharacter : public AITTCharacter
{
	GENERATED_BODY()

	virtual void BeginPlay() override;


	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:


public:
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Grip")
	bool bisGrip;

	UFUNCTION(BlueprintCallable, Category = "Material")
	void MaterialChange();
};
