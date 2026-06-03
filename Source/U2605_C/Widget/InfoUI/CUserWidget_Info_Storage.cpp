#include "Widget/InfoUI/CUserWidget_Info_Storage.h"
#include "Global.h"
#include "Communication/CCommunicationSubsystem_UI.h"

void UCUserWidget_Info_Storage::NativeConstruct()
{
    Super::NativeConstruct();

    UGameInstance* game = GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(commuSubsystem_UI);

    commuSubsystem_UI->GetOnStorageInfoUpdatedDel().AddDynamic(this, &UCUserWidget_Info_Storage::OnStorageInfoUpdatedInternal);
}

void UCUserWidget_Info_Storage::NativeDestruct()
{
    UGameInstance* game = GetGameInstance();
    if (game)
    {
        UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
        if (commuSubsystem_UI)
            commuSubsystem_UI->GetOnStorageInfoUpdatedDel().RemoveAll(this);
    }

    Super::NativeDestruct();
}

void UCUserWidget_Info_Storage::OnStorageInfoUpdatedInternal(const FStorageInfoData& InStorageInfoData)
{
    OnStorageInfoUpdated(InStorageInfoData);
}
