﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InterfaceGrip.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterfaceGrip : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class DONTFORGETME_API IInterfaceGrip
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void BeginMoveToTarget(AActor* TargetActor, FVector TargetLocation) = 0;
	virtual void StopMoveToTarget() = 0;

	// Grip 
	virtual void Grip() = 0;
	virtual void Grip_Implementation() = 0;

	// Drop 
	virtual void Drop() = 0;
	virtual void Drop_Implementation() = 0;
};
