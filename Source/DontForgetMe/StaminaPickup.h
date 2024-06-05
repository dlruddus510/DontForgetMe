
#pragma once

#include "CoreMinimal.h"
#include "RecoveryPickup.h"
#include "StaminaPickup.generated.h"


UCLASS()
class DONTFORGETME_API AStaminaPickup : public ARecoveryPickup
{
    GENERATED_BODY()

public:
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
