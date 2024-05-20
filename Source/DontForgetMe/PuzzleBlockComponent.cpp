// PuzzleBlockComponent.cpp

#include "PuzzleBlockComponent.h"
#include "PuzzleBoard.h" // APuzzleBoard 헤더 추가

UPuzzleBlockComponent::UPuzzleBlockComponent()
{
    // Set default values

    PrimaryComponentTick.bCanEverTick = true;

    bInPlace = false;
    bisOverlap = false;
}

void UPuzzleBlockComponent::BeginPlay()
{
    Super::BeginPlay();

    // Get the owner actor
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        // Handle the case where the owner actor is invalid
        UE_LOG(LogTemp, Warning, TEXT("Owner actor is invalid"));
        return;
    }

    // Get the primitive component
    UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(OwnerActor->GetComponentByClass(UPrimitiveComponent::StaticClass()));
    if (!PrimitiveComponent)
    {
        // Handle the case where the primitive component is not found
        UE_LOG(LogTemp, Warning, TEXT("Primitive component not found on owner actor"));
        return;
    }

    // Subscribe to overlap events
    PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &UPuzzleBlockComponent::OnOverlapBegin);
}


void UPuzzleBlockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UE_LOG(LogTemp, Warning, TEXT("Tick"));

    // Check if the block is in place
    CheckIfInPlace();
}

void UPuzzleBlockComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlap detected with actor: %s"), *OtherActor->GetName());

        // Check if the overlapping actor is a PuzzleBoard
        APuzzleBoard* PuzzleBoard = Cast<APuzzleBoard>(OtherActor);
        if (PuzzleBoard)
        {
            UE_LOG(LogTemp, Warning, TEXT("PuzzleBoard detected!"));
            // Set the overlapping board
            OverlappingBoard = PuzzleBoard;
        }
    }
}

void UPuzzleBlockComponent::CheckIfInPlace()
{
    if (OverlappingBoard &&
        GetOwner()->GetActorLocation().Equals(OverlappingBoard->TargetLocation, 20.0f) &&
        GetOwner()->GetActorRotation().Equals(OverlappingBoard->TargetRotation, 20.0f) &&
        BlockShape == OverlappingBoard->TargetShape)
    {
        bInPlace = true;
        // Optionally notify the puzzle system that this block is in place
    }
}

bool UPuzzleBlockComponent::IsInPlace() const
{
    return bInPlace;
}
bool UPuzzleBlockComponent::IsOverlap() const
{
    return bisOverlap;
}
