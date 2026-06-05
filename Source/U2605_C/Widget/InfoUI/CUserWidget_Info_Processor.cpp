#include "Widget/InfoUI/CUserWidget_Info_Processor.h"
#include "Global.h"
#include "Communication/CCommunicationSubsystem_UI.h"

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

void UCUserWidget_Info_Processor::OnProcessorInfoUpdatedInternal(const FProcessorInfoData& InProcessorInfoData)
{
    OnProcessorInfoUpdated(InProcessorInfoData);
}
