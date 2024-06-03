#include "PuzzleBoard.h"


APuzzleBoard::APuzzleBoard()
{

}
void APuzzleBoard::BeginPlay()
{
    Super::BeginPlay();

    TargetLocation = GetActorLocation();

    TargetRotation = GetActorRotation();
}
