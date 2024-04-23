// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ITTCharacter.h"
#include "RobotCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ITT_API ARobotCharacter : public AITTCharacter
{
	GENERATED_BODY()
	
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
