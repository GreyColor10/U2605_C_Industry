#include "Widget/InfoUI/CInfoUIActor.h"
#include "Global.h"
#include "GameFramework/PlayerController.h"

#include "Widget/InfoUI/CUserWidget_Info_Storage.h"
#include "Widget/InfoUI/CUserWidget_Info_Processor.h"
#include "Interface/IClickable.h"
#include "Communication/CCommunicationSubsystem_UI.h"

ACInfoUIActor::ACInfoUIActor()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACInfoUIActor::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* playerCon = GetWorld()->GetFirstPlayerController();
    CheckNotValid(playerCon);

    if (StorageWidgetClass)
    {
        StorageWidget = CreateWidget<UCUserWidget_Info_Storage>(playerCon, StorageWidgetClass);
        if (StorageWidget)
        {
            StorageWidget->AddToViewport();
            StorageWidget->SetVisibility(ESlateVisibility::Collapsed);
        }   
    }

    if (ProcessorWidgetClass)
    {
        ProcessorWidget = CreateWidget<UCUserWidget_Info_Processor>(playerCon, ProcessorWidgetClass);
        if (ProcessorWidget)
        {
            ProcessorWidget->AddToViewport();
            ProcessorWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    SetActorTickEnabled(false);

    UWorld* world = GetWorld();
    CheckNotValid(world);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(commuSubsystem_UI);

    commuSubsystem_UI->GetOnUITargetChangedDel().BindUObject(this, &ACInfoUIActor::SetTarget);
    commuSubsystem_UI->GetOnUITargetGotten().BindUObject(this, &ACInfoUIActor::GetTarget);
}

void ACInfoUIActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateWidgetPosition();
}

void ACInfoUIActor::SetTarget(AActor* InActor)
{
    if (TargetActor == InActor)
    {
        Hide();
        return;
    }

    Hide();
    TargetActor = InActor;

    IIClickable* clickable = Cast<IIClickable>(TargetActor.Get());
    CheckNull(clickable);

    EInfoUIType uiType = clickable->GetInfoUIType();
    switch (uiType)
    {
    case EInfoUIType::None:
        ActiveWidget = nullptr;
        break;

    case EInfoUIType::Storage:
        ActiveWidget = StorageWidget;
        break;

    case EInfoUIType::Processor:
        ActiveWidget = ProcessorWidget;
        break;
    }

    CheckNotValid(ActiveWidget);
    ActiveWidget->SetVisibility(ESlateVisibility::Visible);
    SetActorTickEnabled(true);
}

void ACInfoUIActor::Hide()
{
    if (ActiveWidget)
        ActiveWidget->SetVisibility(ESlateVisibility::Collapsed);

    TargetActor = nullptr;
    ActiveWidget = nullptr;
    SetActorTickEnabled(false);
}

const AActor* ACInfoUIActor::GetTarget() const
{
    CheckFalseResult(TargetActor.IsValid(), nullptr);
    return TargetActor.Get();
}

void ACInfoUIActor::UpdateWidgetPosition()
{
    CheckNull(ActiveWidget);
    CheckFalse(TargetActor.IsValid());

    APlayerController* playerCon = GetWorld()->GetFirstPlayerController();
    CheckNotValid(playerCon);

    IIClickable* clickable = Cast<IIClickable>(TargetActor.Get());
    CheckNull(clickable);

    float zPosition = clickable->GetUIZOffset();
    
    FVector worldPos = TargetActor->GetActorLocation() + FVector(0, 0, zPosition);
    FVector2D screenPos;

    bool bProjected = playerCon->ProjectWorldLocationToScreen(worldPos, screenPos);
    CheckFalse(bProjected);

    float dpiScale = GetWorld()->GetGameViewport()->GetDPIScale();
    screenPos /= dpiScale;

    FVector2D widgetSize = ActiveWidget->GetDesiredSize();
    screenPos.X -= widgetSize.X / 2.f;
    screenPos.Y -= widgetSize.Y;

    ActiveWidget->SetPositionInViewport(screenPos, true);
}