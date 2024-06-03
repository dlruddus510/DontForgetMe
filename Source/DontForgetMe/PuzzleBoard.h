
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleBlockComponent.h" 
#include "PuzzleBoard.generated.h"

UCLASS()
class DONTFORGETME_API APuzzleBoard : public AActor
{
    GENERATED_BODY()

public:
    APuzzleBoard();

    virtual void BeginPlay() override;
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
        FVector TargetLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
        FRotator TargetRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
        EPuzzleBlockShape TargetShape; 

};
