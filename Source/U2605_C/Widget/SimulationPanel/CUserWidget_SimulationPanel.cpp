#include "Widget/SimulationPanel/CUserWidget_SimulationPanel.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"

void UCUserWidget_SimulationPanel::OnScenarioDeactivedInternal()
{
    OnScenarioDeactived();
}

void UCUserWidget_SimulationPanel::OnScenarioRemainingUpdatedInternal(float InRemainingSeconds)
{
    OnScenarioRemainingUpdated(InRemainingSeconds);
}

void UCUserWidget_SimulationPanel::NativeConstruct()
{
    Super::NativeConstruct();

    UGameInstance* game = GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->GetOnScenarioDeactived().AddDynamic(this, &UCUserWidget_SimulationPanel::OnScenarioDeactivedInternal);
    uiSubsystem->GetOnScenarioRemainingUpdated().AddDynamic(this, &UCUserWidget_SimulationPanel::OnScenarioRemainingUpdatedInternal);
}

void UCUserWidget_SimulationPanel::NativeDestruct()
{
    UGameInstance* game = GetGameInstance();
    if (game)
    {
        UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
        if (uiSubsystem)
        {
            uiSubsystem->GetOnScenarioDeactived().RemoveAll(this);
            uiSubsystem->GetOnScenarioRemainingUpdated().RemoveAll(this);
        }
    }

    Super::NativeDestruct();
}
