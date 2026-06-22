#include "Widget/InfoUI/CUserWidget_Info_Processor.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"

void UCUserWidget_Info_Processor::OnProcessorInfoUpdatedInternal(const FProcessorInfoData& InProcessorInfoData)
{
    OnProgressUpdated(InProcessorInfoData.Progress);
    OnProcessorInfoUpdated(InProcessorInfoData);
}

void UCUserWidget_Info_Processor::OnProcessorProgressUpdatedInternal(float InProgress)
{
    OnProgressUpdated(InProgress);
}

void UCUserWidget_Info_Processor::RequestProcessingTimeChange(float InProcessingTime)
{
    UGameInstance* game = GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    const AActor* target = uiSubsystem->GetCurrentUITarget();
    CheckNull(target);

    uiSubsystem->BroadcastOnProcessingTimeChangeRequested(target->GetClass(), InProcessingTime);
}

void UCUserWidget_Info_Processor::RequestProcessingTimeChangeEnd()
{
    UGameInstance* game = GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->BroadcastOnProcessingTimeChangeEnded();
}

void UCUserWidget_Info_Processor::NativeConstruct()
{
    Super::NativeConstruct();

    UGameInstance* game = GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->GetOnProcessorInfoUpdatedDel().AddDynamic(this, &UCUserWidget_Info_Processor::OnProcessorInfoUpdatedInternal);
    uiSubsystem->GetOnProcessorProgressUpdatedDel().AddDynamic(this, &UCUserWidget_Info_Processor::OnProcessorProgressUpdatedInternal);
}

void UCUserWidget_Info_Processor::NativeDestruct()
{
    UGameInstance* game = GetGameInstance();
    if (game)
    {
        UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
        if (uiSubsystem)
        {
            uiSubsystem->GetOnProcessorInfoUpdatedDel().RemoveAll(this);
            uiSubsystem->GetOnProcessorProgressUpdatedDel().RemoveAll(this);
        }
    }

    Super::NativeDestruct();
}