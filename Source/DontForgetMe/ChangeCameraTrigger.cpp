// Fill out your copyright notice in the Description page of Project Settings.

#include "ChangeCameraTrigger.h"

// Sets default values
AChangeCameraTrigger::AChangeCameraTrigger()
{

    PrimaryActorTick.bCanEverTick = true;

    // 트리거 박스 설정
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
    TriggerBox->InitBoxExtent(FVector(50.0f, 50.0f, 50.0f));
    TriggerBox->SetRelativeLocation(FVector(0.0f, 0.0f, 25.0f));

    RootComponent = TriggerBox;
}


void AChangeCameraTrigger::BeginPlay()
{
    Super::BeginPlay();
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AChangeCameraTrigger::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AChangeCameraTrigger::OnOverlapEnd);
}

void AChangeCameraTrigger::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AChangeCameraTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Overlap Begin Triggered"));
    if (OtherActor && OtherActor != this && CameraToSwitchTo)
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
}

void AChangeCameraTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("Overlap End: %s exited trigger, Sweep: %s"), *OtherActor->GetName());
    if (OtherActor && OtherActor != this)
    {
        // 멀티플레이어 화면 분할 재활성화
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
}

