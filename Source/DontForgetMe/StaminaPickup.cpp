#include "StaminaPickup.h"

void AStaminaPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    AITTCharacter* ITTCharacter = Cast<AITTCharacter>(OtherActor);
    if (ITTCharacter && ITTCharacter->CurrentStamina != ITTCharacter->MaxStamina)
    {
        ITTCharacter->PlayHealingSound();
        ITTCharacter->CurrentStamina += 3;
        ITTCharacter->RestoreWalkSpeed();

         UE_LOG(LogTemp, Warning, TEXT("Stamina Pickup"));
        Destroy(); 
    }
}
