#include "PuzzleBoard.h"

// Sets default values
APuzzleBoard::APuzzleBoard()
{

}
void APuzzleBoard::BeginPlay()
{
    Super::BeginPlay();

    // Set targetLocation to the initial location of the puzzle board
    TargetLocation = GetActorLocation();

    // Set targetRotation to the initial rotation of the puzzle board
    TargetRotation = GetActorRotation();
}
