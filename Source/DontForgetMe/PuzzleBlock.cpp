// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleBlock.h"

APuzzleBlock::APuzzleBlock()
{
    PrimaryActorTick.bCanEverTick = true;
    bInPlace = false;
}

void APuzzleBlock::BeginPlay()
{
    Super::BeginPlay();

    InitialLocation = GetActorLocation();
    InitialRotation = GetActorRotation();
}

void APuzzleBlock::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bInPlace)
    {
        // Check if the block is in place based on its location and rotation
        // Implement your own logic to determine when the block is in its correct position
        if (GetActorLocation().Equals(InitialLocation, 5.0f) && GetActorRotation().Equals(InitialRotation, 5.0f))
        {
            bInPlace = true;
            // Notify the puzzle system that this block is in place
        }
    }
}

bool APuzzleBlock::IsInPlace() const
{
    return bInPlace;
}