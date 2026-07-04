#include "Widget/ScenarioPanel/CUserWidget_ScenarioPanel.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"

void UCUserWidget_ScenarioPanel::OnScenarioDeactivedInternal()
{
    OnScenarioDeactived();
}

void UCUserWidget_ScenarioPanel::OnScenarioRemainingUpdatedInternal(float InRemainingSeconds)
{
    OnScenarioRemainingUpdated(InRemainingSeconds);
}

void UCUserWidget_ScenarioPanel::OnScenarioComparisonReadyInternal(const FScenarioComparisonResult& InResult)
{
    OnScenarioComparisonReady(InResult);
}

void UCUserWidget_ScenarioPanel::BroadcastShortageScenarioStarted(const float InShortageDuration)
{
    UGameInstance* game = GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->BroadcastOnShortageScenarioStarted(InShortageDuration);
}

void UCUserWidget_ScenarioPanel::BroadcastScenarioResultExported()
{
    UGameInstance* game = GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->BroadcastOnScenarioResultExported();
}

void UCUserWidget_ScenarioPanel::NativeConstruct()
{
    Super::NativeConstruct();

    UGameInstance* game = GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->GetOnScenarioDeactived().AddDynamic(this, &UCUserWidget_ScenarioPanel::OnScenarioDeactivedInternal);
    uiSubsystem->GetOnScenarioRemainingUpdated().AddDynamic(this, &UCUserWidget_ScenarioPanel::OnScenarioRemainingUpdatedInternal);
    uiSubsystem->GetOnScenarioComparisonReady().AddDynamic(this, &UCUserWidget_ScenarioPanel::OnScenarioComparisonReadyInternal);
}

void UCUserWidget_ScenarioPanel::NativeDestruct()
{
    UGameInstance* game = GetGameInstance();
    if (game)
    {
        UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
        if (uiSubsystem)
        {
            uiSubsystem->GetOnScenarioDeactived().RemoveAll(this);
            uiSubsystem->GetOnScenarioRemainingUpdated().RemoveAll(this);
            uiSubsystem->GetOnScenarioComparisonReady().RemoveAll(this);
        }
    }

    Super::NativeDestruct();
}
