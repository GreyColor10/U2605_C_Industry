#include "Widget/InfoUI/CInfoUIActor.h"
#include "Global.h"
#include "GameFramework/PlayerController.h"

#include "Widget/InfoUI/CUserWidget_Info_Storage.h"
#include "Widget/InfoUI/CUserWidget_Info_Processor.h"
#include "ProductionEquipment/ProductionEquipment_Base/CProductionEquipment_Storage.h"
#include "ProductionEquipment/ProductionEquipment_Base/CProductionEquipment_Processor.h"

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

    if (Cast<ACProductionEquipment_Storage>(InActor))
        ActiveWidget = StorageWidget;
    else if (Cast<ACProductionEquipment_Processor>(InActor))
        ActiveWidget = ProcessorWidget;
    else
        return;

    CheckNotValid(ActiveWidget);
    ActiveWidget->SetVisibility(ESlateVisibility::Visible);
    SetActorTickEnabled(true);
}

void ACInfoUIActor::Hide()
{
    UCUserWidget_Info_Processor* processorWidget = 
        Cast<UCUserWidget_Info_Processor>(ActiveWidget);

    if (processorWidget)
        processorWidget->ResetState();

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

    float zPosition = 0.0f;
    if (ActiveWidget == StorageWidget) zPosition = 170.0f;
    else if (ActiveWidget == ProcessorWidget) zPosition = 400.0f;

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