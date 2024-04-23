// Fill out your copyright notice in the Description page of Project Settings.


#include "ConductorGripObject.h"


AConductorGripObject::AConductorGripObject()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AConductorGripObject::BeginPlay()
{
    Super::BeginPlay();
    PSC = nullptr; // 참조 초기화

    electricityStatus = false;
}

void AConductorGripObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AConductorGripObject::ConductElectricity()
{
    GetWorld()->GetTimerManager().ClearTimer(ParticleSystemDeactivationTimerHandle);
    if (ElectricEffect&& !electricityStatus  && !PSC)
    {
        electricityStatus = true;
        UE_LOG(LogTemp, Error, TEXT("????"));
        //현재 액터의 위치에서 파티클 시스템 스폰
        PSC = UGameplayStatics::SpawnEmitterAttached(
            ElectricEffect,
            this->GetRootComponent(), // 현재 액터의 루트 컴포넌트에 붙입니다.
            NAME_None,
            GetActorLocation(),
            GetActorRotation(),
            EAttachLocation::KeepWorldPosition,
            true
        );
        FVector CurrentScale = this->GetActorScale3D();
        // 액터의 스케일에 1.5를 곱합니다.
        FVector NewScale = CurrentScale * 3.0f;
        // 파티클 시스템의 스케일을 조정합니다.
        PSC->SetRelativeScale3D(NewScale);
    }
}

void AConductorGripObject::StopConductElectricity()
{
    if (electricityStatus && PSC)
    {
        // 3초 뒤에 DeactivateAndDestroyParticleSystem 함수를 호출합니다.
        electricityStatus = false;
        GetWorld()->GetTimerManager().SetTimer(ParticleSystemDeactivationTimerHandle, this, &AConductorGripObject::DeactivateAndDestroyParticleSystem, 3.0f, false);
    }
}

void AConductorGripObject::DeactivateAndDestroyParticleSystem()
{
    if (PSC)
    {
        PSC->DeactivateSystem(); // 파티클 시스템 비활성화
        PSC->DestroyComponent(); // 컴포넌트 제거
        PSC = nullptr; // 참조 초기화
    }
}