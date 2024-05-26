// PuzzleBoard.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleBlockComponent.h" // EPuzzleBlockShape를 사용하기 위해 퍼즐 블록 헤더를 포함시킴
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
        EPuzzleBlockShape TargetShape; // EPuzzleBlockShape를 사용하여 TargetShape를 정의

};
