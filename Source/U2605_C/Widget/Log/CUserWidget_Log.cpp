#include "Widget/Log/CUserWidget_Log.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"

void UCUserWidget_Log::NativeConstruct()
{
    Super::NativeConstruct();

    UGameInstance* game = GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(commuSubsystem_UI);

    commuSubsystem_UI->GetOnLogEntryAddedDel().AddDynamic(this, &UCUserWidget_Log::OnLogEntryAddedInternal);
}

void UCUserWidget_Log::NativeDestruct()
{
    UGameInstance* game = GetGameInstance();
    if (game)
    {
        UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
        if (commuSubsystem_UI)
            commuSubsystem_UI->GetOnLogEntryAddedDel().RemoveAll(this);
    }

    Super::NativeDestruct();
}

void UCUserWidget_Log::OnLogEntryAddedInternal(const FLogEntry& InLogEntry)
{
    OnLogEntryAdded(InLogEntry);
}
