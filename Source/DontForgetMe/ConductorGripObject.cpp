

#include "ConductorGripObject.h"


AConductorGripObject::AConductorGripObject()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AConductorGripObject::BeginPlay()
{
    Super::BeginPlay();
    PSC = nullptr; 

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

        PSC = UGameplayStatics::SpawnEmitterAttached(
            ElectricEffect,
            this->GetRootComponent(),
            NAME_None,
            GetActorLocation(),
            GetActorRotation(),
            EAttachLocation::KeepWorldPosition,
            true
        );
        FVector CurrentScale = this->GetActorScale3D();

        FVector NewScale = CurrentScale * 3.0f;

        PSC->SetRelativeScale3D(NewScale);
    }
}

void AConductorGripObject::StopConductElectricity()
{
    if (electricityStatus && PSC)
    {

        electricityStatus = false;
        GetWorld()->GetTimerManager().SetTimer(ParticleSystemDeactivationTimerHandle, this, &AConductorGripObject::DeactivateAndDestroyParticleSystem, 3.0f, false);
    }
}

void AConductorGripObject::DeactivateAndDestroyParticleSystem()
{
    if (PSC)
    {
        PSC->DeactivateSystem(); 
        PSC->DestroyComponent(); 
        PSC = nullptr; 
    }
}