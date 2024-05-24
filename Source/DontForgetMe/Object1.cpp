#include "Object1.h"
#include "Robot.h"
#include "Object2.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"
#include "TimerManager.h"

// Sets default values
AObject1::AObject1()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComponent"));
    RootComponent = MeshComp;
    MeshComp->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube'")).Object);
    MeshComp->SetCollisionProfileName(TEXT("BlockAll"));
    MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);

    MeshComp->OnComponentBeginOverlap.AddDynamic(this, &AObject1::OnOverlapBegin);

    bIsMoving = false;
    bIsReturning = false;
    MoveSpeed = 100.0f;
    MoveDistance = 100.0f;

    NewMaterial = nullptr;
    OriginalMaterial = nullptr;
}

// Called when the game starts or when spawned
void AObject1::BeginPlay()
{
    Super::BeginPlay();
    InitialLocation = GetActorLocation();
    TargetLocation = InitialLocation + FVector(MoveDistance, 0.0f, 0.0f);

    if (MeshComp->GetMaterial(0))
    {
        OriginalMaterial = MeshComp->GetMaterial(0);
    }

    if (TargetObject2)
    {
        Object2InitialLocation = TargetObject2->GetActorLocation();
    }
}

// Called every frame
void AObject1::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsMoving)
    {
        FVector ForwardVector = FVector(1.0f, 0.0f, 0.0f);
        FVector NewLocation = GetActorLocation() + (ForwardVector * MoveSpeed * DeltaTime);

        if (FVector::Dist(InitialLocation, NewLocation) < MoveDistance)
        {
            SetActorLocation(NewLocation);

            if (TargetObject2)
            {
                FVector TargetNewLocation = TargetObject2->GetActorLocation() + (ForwardVector * MoveSpeed * DeltaTime);
                TargetObject2->SetActorLocation(TargetNewLocation);
            }
        }
        else
        {
            bIsMoving = false;

            // Set timer to start return movement after 3 seconds
            GetWorld()->GetTimerManager().SetTimer(ReturnTimerHandle, this, &AObject1::StartReturnMovement, 3.0f, false);

            // Set timer to reset the material after 3 seconds
            GetWorld()->GetTimerManager().SetTimer(MaterialTimerHandle, this, &AObject1::ResetMaterial, 2.9f, false);
        }
    }
    else if (bIsReturning)
    {
        ReturnObjects(DeltaTime);
    }
}

void AObject1::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Overlap Event Triggered with %s"), *OtherActor->GetName());

    if (!bIsMoving && OtherActor && OtherActor->IsA(ARobot::StaticClass()))
    {
        bIsMoving = true;
        InitialLocation = GetActorLocation();

        if (NewMaterial)
        {
            MeshComp->SetMaterial(0, NewMaterial);
        }
    }
}

void AObject1::MoveObjects()
{
    bIsMoving = true;
    InitialLocation = GetActorLocation();
}

void AObject1::StartReturnMovement()
{
    bIsReturning = true;
}

void AObject1::ReturnObjects(float DeltaTime)
{
    FVector BackwardVector = FVector(-1.0f, 0.0f, 0.0f);
    FVector NewLocation = GetActorLocation() + (BackwardVector * MoveSpeed * DeltaTime);

    if (FVector::Dist(TargetLocation, NewLocation) < MoveDistance)
    {
        SetActorLocation(NewLocation);

        if (TargetObject2)
        {
            FVector TargetNewLocation = TargetObject2->GetActorLocation() + (BackwardVector * MoveSpeed * DeltaTime);
            TargetObject2->SetActorLocation(TargetNewLocation);
        }
    }
    else
    {
        bIsReturning = false;

        ResetObjects();
    }
}

void AObject1::ResetObjects()
{
    if (TargetObject2)
    {
        TargetObject2->SetActorLocation(Object2InitialLocation);
    }
}

void AObject1::ResetMaterial()
{
    if (OriginalMaterial)
    {
        MeshComp->SetMaterial(0, OriginalMaterial);
    }
}
