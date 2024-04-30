// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ConductorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UConductorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DONTFORGETME_API IConductorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// 전기가 통하는 기능을 실행하는 함수
	virtual void ConductElectricity() = 0;
	virtual void StopConductElectricity() = 0;
	virtual void DeactivateAndDestroyParticleSystem() = 0;
};
