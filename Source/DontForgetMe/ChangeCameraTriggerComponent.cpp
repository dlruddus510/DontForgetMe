

#include "ChangeCameraTriggerComponent.h"


UChangeCameraTriggerComponent::UChangeCameraTriggerComponent()
{

    PrimaryComponentTick.bCanEverTick = true;
}


void UChangeCameraTriggerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UChangeCameraTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType,ThisTickFunction);
}


void UChangeCameraTriggerComponent::ChangeCamera()
{
    if (CameraToSwitchTo)
    {
   
        UE_LOG(LogTemp, Warning, TEXT("Attempting to switch camera"));
        UGameplayStatics::GetGameInstance(this)->GetWorld()->GetGameViewport()->SetForceDisableSplitscreen(true);


        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* PlayerController = It->Get();
            if (PlayerController)
            {
                PlayerController->SetViewTargetWithBlend(CameraToSwitchTo, 0.5f);  
            }
        }
    }

}

void UChangeCameraTriggerComponent::EndChangeCamera()
{
    UGameplayStatics::GetGameInstance(this)->GetWorld()->GetGameViewport()->SetDisableSplitscreenOverride(false);

 
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PlayerController = It->Get();
        if (PlayerController && PlayerController->GetPawn())
        {
            PlayerController->SetViewTargetWithBlend(PlayerController->GetPawn(), 0.5f);
        }
    }
}