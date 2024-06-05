
#include "RecoveryPickup.h"

ARecoveryPickup::ARecoveryPickup()
{

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;


    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ARecoveryPickup::OnOverlapBegin);

}

void ARecoveryPickup::BeginPlay()
{
    Super::BeginPlay();
}

void ARecoveryPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ARecoveryPickup::PlaySound()
{

}