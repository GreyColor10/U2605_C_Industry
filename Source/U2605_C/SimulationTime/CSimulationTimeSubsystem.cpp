#include "SimulationTime/CSimulationTimeSubsystem.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"

void UCSimulationTimeSubsystem::ChangeSimulationState()
{
    if (bIsRunning) StopSimulation();
    else StartSimulation();
}

void UCSimulationTimeSubsystem::StartSimulation()
{
    CheckTrue(bIsRunning);
    bIsRunning = true;

    UWorld* world = GetWorld();
    CheckNotValid(world);

    SimulationStartTime = world->GetTimeSeconds() - PausedElapsedSeconds;

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(commuSubsystem_UI);

    commuSubsystem_UI->BroadcastOnSimulationStateChanged(true);
}

void UCSimulationTimeSubsystem::StopSimulation()
{
    CheckFalse(bIsRunning);
    bIsRunning = false;

    UWorld* world = GetWorld();
    CheckNotValid(world);

    PausedElapsedSeconds = world->GetTimeSeconds() - SimulationStartTime;

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(commuSubsystem_UI);

    commuSubsystem_UI->BroadcastOnSimulationStateChanged(false);
}