// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotCharacter.h"

void ARobotCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	//PlayerInputComponent->BindAction("Grip", IE_Pressed, this, &ARobotCharacter::GripAndDrop);

	//PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &ARobotCharacter::Throw);
}