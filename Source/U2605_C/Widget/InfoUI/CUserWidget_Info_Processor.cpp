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
    commuSubsystem_UI->GetOnProcessorProgressUpdatedDel().AddDynamic(this, &UCUserWidget_Info_Processor::OnProcessorProgressUpdatedInternal);
}

void UCUserWidget_Info_Processor::NativeDestruct()
{
    UGameInstance* game = GetGameInstance();
    if (game)
    {
        UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
        if (commuSubsystem_UI)
        {
            commuSubsystem_UI->GetOnProcessorInfoUpdatedDel().RemoveAll(this);
            commuSubsystem_UI->GetOnProcessorProgressUpdatedDel().RemoveAll(this);
        }
    }

    Super::NativeDestruct();
}