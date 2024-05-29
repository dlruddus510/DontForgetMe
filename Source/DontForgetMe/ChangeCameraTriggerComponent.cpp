// Fill out your copyright notice in the Description page of Project Settings.

#include "ChangeCameraTriggerComponent.h"

// Sets default values
UChangeCameraTriggerComponent::UChangeCameraTriggerComponent()
{

    PrimaryComponentTick.bCanEverTick = true;

    // 트리거 박스 설정
   /* TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
    TriggerBox->InitBoxExtent(FVector(50.0f, 50.0f, 50.0f));
    TriggerBox->SetRelativeLocation(FVector(0.0f, 0.0f, 25.0f));

    RootComponent = TriggerBox;*/
}


void UChangeCameraTriggerComponent::BeginPlay()
{
    Super::BeginPlay();
/*    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &UChangeCameraTriggerComponent::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &UChangeCameraTriggerComponent::OnOverlapEnd)*/;
}

void UChangeCameraTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType,ThisTickFunction);
}


void UChangeCameraTriggerComponent::ChangeCamera()
{
    if (CameraToSwitchTo)
    {
        // 멀티플레이어 화면 분할 해제
        UE_LOG(LogTemp, Warning, TEXT("Attempting to switch camera"));
        UGameplayStatics::GetGameInstance(this)->GetWorld()->GetGameViewport()->SetForceDisableSplitscreen(true);

        // 모든 로컬 플레이어 컨트롤러의 시점 변경
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* PlayerController = It->Get();
            if (PlayerController)
            {
                PlayerController->SetViewTargetWithBlend(CameraToSwitchTo, 0.5f);  // 블렌드 시간 0.5초
            }
        }
    }
   // GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UChangeCameraTriggerComponent::EndChangeCamera, 5.0f, false);
}

void UChangeCameraTriggerComponent::EndChangeCamera()
{
    UGameplayStatics::GetGameInstance(this)->GetWorld()->GetGameViewport()->SetDisableSplitscreenOverride(false);

        // 모든 로컬 플레이어 컨트롤러의 시점을 기본 카메라(일반적으로 각 플레이어의 캐릭터)로 변경
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PlayerController = It->Get();
        if (PlayerController && PlayerController->GetPawn())
        {
            PlayerController->SetViewTargetWithBlend(PlayerController->GetPawn(), 0.5f);
        }
    }
}