// Fill out your copyright notice in the Description page of Project Settings.


#include "Object2.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
AObject2::AObject2()
{
	//	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	   //PrimaryActorTick.bCanEverTick = true;

	   //// Create and configure a mesh component so we can see where our cube is
	   //RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComponent"));
	   //Cast<UStaticMeshComponent>(RootComponent)->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube'")).Object);
	   //Cast<UStaticMeshComponent>(RootComponent)->SetCollisionProfileName(TEXT("OverlapAll"));
	   //Cast<UStaticMeshComponent>(RootComponent)->OnComponentBeginOverlap.AddDynamic(this, &AObject2::OnOverlapBegin);
	   //StaticMesh'/Game/StarterContent/Shapes/Shape_Plane


	UStaticMeshComponent* MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComponent"));
	RootComponent = MeshComp;

	MeshComp->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube'")).Object);
	MeshComp->SetCollisionProfileName(TEXT("BlockAll"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);


	//	MeshComp->OnComponentBeginOverlap.AddDynamic(this, &AObject2::OnOverlapBegin);


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

void AObject2::MoveForward(float Distance)
{
	FVector NewLocation = GetActorLocation() + FVector(Distance, 0, 0);
	SetActorLocation(NewLocation);
}