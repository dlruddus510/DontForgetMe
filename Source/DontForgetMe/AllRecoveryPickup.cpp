
#include "AllRecoveryPickup.h"

void AAllRecoveryPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AITTCharacter* Character = Cast<AITTCharacter>(OtherActor);
    if (Character)
    {
        if (Character->CurrentStamina != Character->MaxStamina || Character->CurrentHealth != Character->MaxHealth)
        {
            Character->PlayHealingSound();
            Character->CurrentHealth = Character->MaxHealth;
            Character->CurrentStamina = Character->MaxStamina;
            Character->RestoreWalkSpeed();
            GetWorld()->GetTimerManager().ClearTimer(Character->SlowStateTimer);
            UE_LOG(LogTemp, Warning, TEXT("All Pickup"));

            Destroy();
        }
    }
}
