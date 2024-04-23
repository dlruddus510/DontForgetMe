// Fill out your copyright notice in the Description page of Project Settings.


#include "GripObject.h"

// Sets default values
AGripObject::AGripObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGripObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGripObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsMovingToTarget && TargetActor)
    {
        FVector CurrentLocation = GetActorLocation();
        FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, 15.0f); // 15�� �ӵ��� �̵�

        SetActorLocation(NewLocation);

        // ��ǥ ��ġ�� ����� ����������� Ȯ���մϴ�.
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
    FVector CurrentScale = GetActorScale3D();
    FVector NewScale = CurrentScale * 0.5; // 현재 스케일의 4분의 1
    SetActorScale3D(NewScale);
    SetActorTickEnabled(true);
}

void AGripObject::StopMoveToTarget()
{
    bIsMovingToTarget = false;
    //FVector CurrentScale = GetActorScale3D();
    //FVector NewScale = CurrentScale ; // 현재 스케일의 4배
    //SetActorScale3D(NewScale);
    SetActorTickEnabled(false);
}

void AGripObject::Drop_Implementation()
{

}

void AGripObject::Grip_Implementation()
{

}
