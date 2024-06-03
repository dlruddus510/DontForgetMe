
#include "GripObject.h"


AGripObject::AGripObject()
{

	PrimaryActorTick.bCanEverTick = true;
}


void AGripObject::BeginPlay()
{
	Super::BeginPlay();
	
}


void AGripObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsMovingToTarget && TargetActor)
    {
        FVector CurrentLocation = GetActorLocation();
        FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, 15.0f); 

        SetActorLocation(NewLocation);


        if (FVector::Dist(NewLocation, TargetLocation) < 1.0f)
        {
            USceneComponent* MyRootComponent = GetRootComponent();
            USceneComponent* TargetActorMesh = Cast<ABearCharacter>(TargetActor)->GetMesh();

            if (RootComponent && TargetActorMesh && !RootComponent->IsAttachedTo(TargetActorMesh))
            {
                RootComponent->AttachToComponent(TargetActorMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("AttachPosition"));
            }
            bIsMovingToTarget = false;
        }
    }
}

void AGripObject::BeginMoveToTarget(AActor* NewTargetActor, FVector NewTargetLocation)
{
    TargetActor = NewTargetActor;
    TargetLocation = NewTargetLocation;
    bIsMovingToTarget = true;

    SetActorTickEnabled(true);
}

void AGripObject::StopMoveToTarget()
{
    bIsMovingToTarget = false;

    SetActorTickEnabled(false);
}

void AGripObject::Drop_Implementation()
{

}

void AGripObject::Grip_Implementation()
{

}
