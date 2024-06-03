
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InterfaceGrip.generated.h"

UINTERFACE(MinimalAPI)
class UInterfaceGrip : public UInterface
{
	GENERATED_BODY()
};


class DONTFORGETME_API IInterfaceGrip
{
	GENERATED_BODY()

public:

	virtual void BeginMoveToTarget(AActor* TargetActor, FVector TargetLocation) = 0;
	virtual void StopMoveToTarget() = 0;


	virtual void Grip() = 0;
	virtual void Grip_Implementation() = 0;

	virtual void Drop() = 0;
	virtual void Drop_Implementation() = 0;
};
