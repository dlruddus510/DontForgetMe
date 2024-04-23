#include "StaminaPickup.h"

void AStaminaPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 캐릭터와 충돌했는지 확인 (AITTCharacter 타입)
    AITTCharacter* ITTCharacter = Cast<AITTCharacter>(OtherActor);
    if (ITTCharacter && ITTCharacter->CurrentStamina != ITTCharacter->MaxStamina)
    {
        ITTCharacter->CurrentStamina += 3;
        ITTCharacter->RestoreWalkSpeed();
        HandleStaminaRecovery(ITTCharacter, SweepResult);
    }
}

// 스태미나 회복 및 파티클 생성을 처리하는 별도의 함수
void AStaminaPickup::HandleStaminaRecovery(AActor* Character, const FHitResult& SweepResult)
{
    FVector SpawnLocation = SweepResult.ImpactPoint; // 충돌 위치
    FRotator SpawnRotation = FRotator::ZeroRotator; // 파티클의 회전값. 필요에 따라 조절 가능

    // 나이아가라 파티클 시스템 애셋 참조. 에디터에서 설정한 애셋으로 교체해야 함
    UNiagaraSystem* MyNiagaraSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/sA_PickupSet_1/Fx/NiagaraSystems/NS_Energy_2"));

    if (MyNiagaraSystem)
    {
        UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MyNiagaraSystem, SpawnLocation, SpawnRotation);
        if (NiagaraComponent)
        {
            NiagaraComponent->SetAutoDestroy(true);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Stamina Pickup"));
    Destroy(); // 회복 아이템은 사용 후 제거됩니다.
}