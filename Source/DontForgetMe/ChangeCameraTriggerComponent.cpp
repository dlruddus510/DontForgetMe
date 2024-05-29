// Fill out your copyright notice in the Description page of Project Settings.

#include "ChangeCameraTriggerComponent.h"

// Sets default values
UChangeCameraTriggerComponent::UChangeCameraTriggerComponent()
{

    PrimaryComponentTick.bCanEverTick = true;

    // Ʈ���� �ڽ� ����
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
        // ��Ƽ�÷��̾� ȭ�� ���� ����
        UE_LOG(LogTemp, Warning, TEXT("Attempting to switch camera"));
        UGameplayStatics::GetGameInstance(this)->GetWorld()->GetGameViewport()->SetForceDisableSplitscreen(true);

        // ��� ���� �÷��̾� ��Ʈ�ѷ��� ���� ����
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* PlayerController = It->Get();
            if (PlayerController)
            {
                PlayerController->SetViewTargetWithBlend(CameraToSwitchTo, 0.5f);  // ���� �ð� 0.5��
            }
        }
    }
   // GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UChangeCameraTriggerComponent::EndChangeCamera, 5.0f, false);
}

void UChangeCameraTriggerComponent::EndChangeCamera()
{
    UGameplayStatics::GetGameInstance(this)->GetWorld()->GetGameViewport()->SetDisableSplitscreenOverride(false);

        // ��� ���� �÷��̾� ��Ʈ�ѷ��� ������ �⺻ ī�޶�(�Ϲ������� �� �÷��̾��� ĳ����)�� ����
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PlayerController = It->Get();
        if (PlayerController && PlayerController->GetPawn())
        {
            PlayerController->SetViewTargetWithBlend(PlayerController->GetPawn(), 0.5f);
        }
    }
}