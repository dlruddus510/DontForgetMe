// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"

void AHealthPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 캐릭터와 충돌했는지 확인
    AITTCharacter* Character = Cast<AITTCharacter>(OtherActor);

    if (Character)
    {
        if (Character->CurrentHealth != Character->MaxHealth)
        {
            Character->PlayHealingSound();
            // 캐릭터의 체력 2회복
            Character->CurrentHealth += 2;
            //  Character->TakeDamage(0, FDamageEvent(), nullptr, nullptr);
            UE_LOG(LogTemp, Warning, TEXT("Health Pickup"));

            // 회복 아이템은 사용 후 제거됩니다.
            Destroy();
        }
    }
    //FVector SpawnLocation = SweepResult.ImpactPoint; // 충돌 위치
    //FRotator SpawnRotation = FRotator::ZeroRotator; // 파티클의 회전값. 필요에 따라 조절 가능

    //// 나이아가라 파티클 시스템 애셋 참조. 에디터에서 설정한 애셋으로 교체해야 함
    //UNiagaraSystem* MyNiagaraSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/sA_PickupSet_1/Fx/NiagaraSystems/NS_Healing_1"));

    //if (MyNiagaraSystem)
    //{
    //    // 파티클 시스템 생성
    //    UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MyNiagaraSystem, SpawnLocation, SpawnRotation);

    //    if (NiagaraComponent)
    //    {
    //        // 파티클이 점점 희미해지는 효과는 나이아가라 시스템의 설정을 통해 구현
    //        // 예를 들어, 파티클의 Color 또는 Alpha 속성을 시간에 따라 변경

    //        // 파티클 시스템이 자동으로 사라지도록 설정
    //        NiagaraComponent->SetAutoDestroy(true); // 파티클의 수명이 다하면 자동으로 컴포넌트를 파괴
    //    }
    //}
}