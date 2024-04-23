// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ITTCharacter.h"
#include "BearCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ITT_API ABearCharacter : public AITTCharacter
{
	GENERATED_BODY()

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	IInterfaceGrip* GrippableActor;

	void GripAndDrop(); //들기&놓기

	void Throw(); //던지기
	
};
