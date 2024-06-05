

#include "HealthPickup.h"

void AHealthPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    AITTCharacter* Character = Cast<AITTCharacter>(OtherActor);

    if (Character)
    {
        if (Character->CurrentHealth != Character->MaxHealth)
        {
            Character->PlayHealingSound();

            Character->CurrentHealth += 2;

            UE_LOG(LogTemp, Warning, TEXT("Health Pickup"));


            Destroy();
        }
    }
}