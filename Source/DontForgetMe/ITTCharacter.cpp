// Fill out your copyright notice in the Description page of Project Settings.


#include "ITTCharacter.h"

// Sets default values
AITTCharacter::AITTCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AITTCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AITTCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AITTCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

