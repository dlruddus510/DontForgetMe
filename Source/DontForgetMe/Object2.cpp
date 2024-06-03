#include "Object2.h"
#include "Components/StaticMeshComponent.h"


AObject2::AObject2()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComponent"));
    RootComponent = MeshComp;

    MeshComp->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube'")).Object);
    MeshComp->SetCollisionProfileName(TEXT("BlockAll"));
    MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);

    MoveDirection = FVector(1.0f, 0.0f, 0.0f); 
}


void AObject2::BeginPlay()
{
    Super::BeginPlay();
}

void AObject2::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AObject2::MoveObject(float Distance)
{
    FVector NewLocation = GetActorLocation() + (MoveDirection * Distance);
    SetActorLocation(NewLocation);
}

void AObject2::SetMoveDirection(FVector NewDirection)
{
    MoveDirection = NewDirection.GetSafeNormal(); 
}
