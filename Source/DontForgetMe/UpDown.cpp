#include "Object1.h"
#include "ITTCharacter.h"
#include "Robot.h"
#include "UpDown.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"
#include "TimerManager.h"

// Sets default values
AUpDown::AUpDown()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComponent"));
    RootComponent = MeshComp;
    MeshComp->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube'")).Object);
    MeshComp->SetCollisionProfileName(TEXT("BlockAll"));
    MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);

    MeshComp->OnComponentBeginOverlap.AddDynamic(this, &AUpDown::OnOverlapBegin);

    bIsMoving = false;
    bIsReturning = false;
    MoveSpeed = 100.0f;
    MoveDistance = 250.0f;

    NewMaterial = nullptr;
    OriginalMaterial = nullptr;

    MoveDirection = FVector(1.0f, 0.0f, 0.0f); // �ʱ� �̵� ������ X������ ����
}

// Called when the game starts or when spawned
void AUpDown::BeginPlay()
{
    Super::BeginPlay();
    InitialLocation = GetActorLocation();
    TargetLocation = InitialLocation + (MoveDirection * MoveDistance);

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
void AUpDown::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsMoving)
    {
        FVector NewLocation = GetActorLocation() + (MoveDirection * MoveSpeed * DeltaTime);

        if (FVector::Dist(InitialLocation, NewLocation) < MoveDistance)
        {
            SetActorLocation(NewLocation);

            if (TargetObject2)
            {
                FVector TargetNewLocation = TargetObject2->GetActorLocation() + (MoveDirection * MoveSpeed * DeltaTime);
                TargetObject2->SetActorLocation(TargetNewLocation);
            }
        }
        else
        {
            bIsMoving = false;

            // Set timer to start return movement after 3 seconds
            GetWorld()->GetTimerManager().SetTimer(ReturnTimerHandle, this, &AUpDown::StartReturnMovement, 3.0f, false);

            // Set timer to reset the material after 3 seconds
            GetWorld()->GetTimerManager().SetTimer(MaterialTimerHandle, this, &AUpDown::ResetMaterial, 2.9f, false);
        }
    }
    else if (bIsReturning)
    {
        ReturnObjects(DeltaTime);
    }
}

void AUpDown::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AUpDown::MoveObjects()
{
    bIsMoving = true;
    InitialLocation = GetActorLocation();
}

void AUpDown::StartReturnMovement()
{
    bIsReturning = true;
}

void AUpDown::ReturnObjects(float DeltaTime)
{
    FVector NewLocation = GetActorLocation() - (MoveDirection * MoveSpeed * DeltaTime);

    if (FVector::Dist(TargetLocation, NewLocation) < MoveDistance)
    {
        SetActorLocation(NewLocation);

        if (TargetObject2)
        {
            FVector TargetNewLocation = TargetObject2->GetActorLocation() - (MoveDirection * MoveSpeed * DeltaTime);
            TargetObject2->SetActorLocation(TargetNewLocation);
        }
    }
    else
    {
        bIsReturning = false;

        ResetObjects();
    }
}

void AUpDown::ResetObjects()
{
    if (TargetObject2)
    {
        TargetObject2->SetActorLocation(Object2InitialLocation);
    }
}

void AUpDown::ResetMaterial()
{
    if (OriginalMaterial)
    {
        MeshComp->SetMaterial(0, OriginalMaterial);
    }
}

// �̵� ������ �����ϴ� �Լ� �߰�
void AUpDown::SetMoveDirection(FVector NewDirection)
{
    MoveDirection = NewDirection.GetSafeNormal(); // ���� ���͸� ���� ���ͷ� ����ȭ
    TargetLocation = InitialLocation + (MoveDirection * MoveDistance);
}