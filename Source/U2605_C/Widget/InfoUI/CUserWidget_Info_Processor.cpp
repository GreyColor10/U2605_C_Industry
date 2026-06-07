#include "Widget/InfoUI/CUserWidget_Info_Processor.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"

void UCUserWidget_Info_Processor::OnProcessorInfoUpdatedInternal(const FProcessorInfoData& InProcessorInfoData)
{
    CachedInfo = InProcessorInfoData;
    bIsProcessing = (InProcessorInfoData.State == EEquipmentState::Processing);

    if (!bIsProcessing)
        OnProgressUpdated(0.0f);

    OnProcessorInfoUpdated(InProcessorInfoData);
}

void UCUserWidget_Info_Processor::RequestProcessingTimeChange(float InProcessingTime)
{
    UGameInstance* game = GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(commuSubsystem_UI);

    const AActor* target = commuSubsystem_UI->GetCurrentUITarget();
    CheckNull(target);

    commuSubsystem_UI->BroadcastOnProcessingTimeChangeRequested(target->GetClass(), InProcessingTime);
}

void UCUserWidget_Info_Processor::NativeConstruct()
{
    Super::NativeConstruct();

    UGameInstance* game = GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(commuSubsystem_UI);

    commuSubsystem_UI->GetOnProcessorInfoUpdatedDel().AddDynamic(this, &UCUserWidget_Info_Processor::OnProcessorInfoUpdatedInternal);
}

void UCUserWidget_Info_Processor::NativeDestruct()
{
    UGameInstance* game = GetGameInstance();
    if (game)
    {
        UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
        if (commuSubsystem_UI)
            commuSubsystem_UI->GetOnProcessorInfoUpdatedDel().RemoveAll(this);
    }

    Super::NativeDestruct();
}

void UCUserWidget_Info_Processor::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    CheckFalse(bIsProcessing);

    UWorld* world = GetWorld();
    CheckNotValid(world);

    float now = world->GetTimeSeconds();
    float remaining = CachedInfo.ProcessingEndTime - now;
    float progress = 1.0f - (remaining / CachedInfo.ProcessingTime);
    progress = FMath::Clamp(progress, 0.0f, 1.0f);

    OnProgressUpdated(progress);
}

void UCUserWidget_Info_Processor::ResetState()
{
    bIsProcessing = false;
    OnProgressUpdated(0.0f);
}

