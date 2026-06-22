#include "Widget/InfoUI/CUserWidget_Info_Storage.h"
#include "Global.h"
#include "Communication/CCommunicationSubsystem_UI.h"

void UCUserWidget_Info_Storage::NativeConstruct()
{
    Super::NativeConstruct();

    UGameInstance* game = GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->GetOnStorageInfoUpdatedDel().AddDynamic(this, &UCUserWidget_Info_Storage::OnStorageInfoUpdatedInternal);
}

void UCUserWidget_Info_Storage::NativeDestruct()
{
    UGameInstance* game = GetGameInstance();
    if (game)
    {
        UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
        if (uiSubsystem)
            uiSubsystem->GetOnStorageInfoUpdatedDel().RemoveAll(this);
    }

    Super::NativeDestruct();
}

void UCUserWidget_Info_Storage::OnStorageInfoUpdatedInternal(const FStorageInfoData& InStorageInfoData)
{
    OnStorageInfoUpdated(InStorageInfoData);
}
