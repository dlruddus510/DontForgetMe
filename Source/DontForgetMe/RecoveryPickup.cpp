// Fill out your copyright notice in the Description page of Project Settings.

#include "RecoveryPickup.h"

ARecoveryPickup::ARecoveryPickup()
{
    // �浹 �ڽ� ������Ʈ ���� �� �ʱ�ȭ
    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;

    // �浹 ������ ���� ����
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    // �̺�Ʈ ���ε�
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ARecoveryPickup::OnOverlapBegin);
}

void ARecoveryPickup::BeginPlay()
{
    Super::BeginPlay();
}

void ARecoveryPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}