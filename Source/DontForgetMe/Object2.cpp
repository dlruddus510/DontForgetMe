#include "Object2.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
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

    MoveDirection = FVector(1.0f, 0.0f, 0.0f); // 초기 이동 방향을 X축으로 설정
}

// Called when the game starts or when spawned
void AObject2::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
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
    MoveDirection = NewDirection.GetSafeNormal(); // 방향 벡터를 단위 벡터로 정규화
}
