
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ConductorInterface.generated.h"


UINTERFACE(MinimalAPI)
class UConductorInterface : public UInterface
{
	GENERATED_BODY()
};


class DONTFORGETME_API IConductorInterface
{
	GENERATED_BODY()


public:

	virtual void ConductElectricity() = 0;
	virtual void StopConductElectricity() = 0;
	virtual void DeactivateAndDestroyParticleSystem() = 0;
};
