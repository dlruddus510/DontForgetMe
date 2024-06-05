
#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "ITTCharacter.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "RecoveryPickup.generated.h"

UCLASS()
class DONTFORGETME_API ARecoveryPickup : public AActor
{
    GENERATED_BODY()

public:

    ARecoveryPickup();

protected:
  
    virtual void BeginPlay() override;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
        USoundBase* PickupSound;

 
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UBoxComponent* CollisionBox;

    UFUNCTION()
        void PlaySound();


    UFUNCTION()
   virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};