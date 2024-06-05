

#include "PuzzleBlockComponent.h"
#include "PuzzleBoard.h"

UPuzzleBlockComponent::UPuzzleBlockComponent()
{


    PrimaryComponentTick.bCanEverTick = true;

    bInPlace = false;
    bInPlaceandShape = false;
    bisOverlap = false;
}

void UPuzzleBlockComponent::BeginPlay()
{
    Super::BeginPlay();


    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
  
        UE_LOG(LogTemp, Warning, TEXT("Owner actor is invalid"));
        return;
    }


    UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(OwnerActor->GetComponentByClass(UPrimitiveComponent::StaticClass()));
    if (!PrimitiveComponent)
    {
  
        UE_LOG(LogTemp, Warning, TEXT("Primitive component not found on owner actor"));
        return;
    }


    PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &UPuzzleBlockComponent::OnOverlapBegin);
}


void UPuzzleBlockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


    CheckIfInPlace();
    CheckIfShape();
}

void UPuzzleBlockComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlap detected with actor: %s"), *OtherActor->GetName());

   
        APuzzleBoard* PuzzleBoard = Cast<APuzzleBoard>(OtherActor);
        if (PuzzleBoard)
        {
            UE_LOG(LogTemp, Warning, TEXT("PuzzleBoard detected!"));

            OverlappingBoard = PuzzleBoard;
        }
    }
}

void UPuzzleBlockComponent::CheckIfInPlace()
{
    if (OverlappingBoard &&
        GetOwner()->GetActorLocation().Equals(OverlappingBoard->TargetLocation, 15.0f) &&
        GetOwner()->GetActorRotation().Equals(OverlappingBoard->TargetRotation, 15.0f) )
    {
        bInPlace = true;
        UE_LOG(LogTemp, Warning, TEXT("bInPlace = true"));
    
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

void UPuzzleBlockComponent::CheckIfShape()
{
    if (OverlappingBoard && bInPlace && BlockShape == OverlappingBoard->TargetShape)
    {
        bInPlaceandShape = true;
    }
}
bool UPuzzleBlockComponent::IsInPlaceAndShape() const
{
    return bInPlaceandShape;
}