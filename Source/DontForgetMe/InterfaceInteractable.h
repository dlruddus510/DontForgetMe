
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InterfaceInteractable.generated.h"


UINTERFACE(MinimalAPI)
class UInterfaceInteractable : public UInterface
{
	GENERATED_BODY()
};


class DONTFORGETME_API IInterfaceInteractable
{
	GENERATED_BODY()

public:
	virtual void Interact(ACharacter* Character) = 0;
};
