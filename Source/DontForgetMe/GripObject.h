
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BearCharacter.h"
#include "InterfaceGrip.h"
#include "GripObject.generated.h"

UCLASS()
class DONTFORGETME_API AGripObject : public AActor, public IInterfaceGrip
{
	GENERATED_BODY()
	
public:	

	AGripObject();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	AActor* TargetActor;
	FVector TargetLocation;
	bool bIsMovingToTarget;


	void BeginMoveToTarget(AActor* NewTargetActor, FVector NewTargetLocation);
	void StopMoveToTarget();



	UFUNCTION(BlueprintNativeEvent, Category = "InterfaceAction")
	void Grip();
	void Grip_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "InterfaceAction")
	void Drop();
	void Drop_Implementation();
};
