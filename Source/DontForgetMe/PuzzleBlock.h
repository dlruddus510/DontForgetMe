// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleBlock.generated.h"

UCLASS()
class DONTFORGETME_API APuzzleBlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APuzzleBlock();

    virtual void Tick(float DeltaTime) override;

    bool IsInPlace() const;

protected:
    virtual void BeginPlay() override;

private:
    FVector InitialLocation;
    FRotator InitialRotation;
    bool bInPlace;
};
