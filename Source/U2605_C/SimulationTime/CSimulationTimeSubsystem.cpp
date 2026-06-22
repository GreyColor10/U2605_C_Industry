#include "SimulationTime/CSimulationTimeSubsystem.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"

void UCSimulationTimeSubsystem::ChangeSimulationState()
{
    if (bIsRunning) StopSimulation();
    else StartSimulation();
}

float UCSimulationTimeSubsystem::GetElapsedSeconds() const
{
    CheckFalseResult(bIsRunning, PausedElapsedSeconds);

    UWorld* world = GetWorld();
    CheckNotValidResult(world, PausedElapsedSeconds);

    return world->GetTimeSeconds() - SimulationStartTime;
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

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    FLogEntry entry;
    entry.EventType = ELogEventType::Info;
    entry.Message = TEXT("시뮬레이션 시작");
    entry.TimestampText = FLogEntry::FormatTimestamp();
    uiSubsystem->BroadcastOnLogEntryAdded(entry);

    uiSubsystem->BroadcastOnSimulationStateChanged(true);
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

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    FLogEntry entry;
    entry.EventType = ELogEventType::Info;
    entry.Message = TEXT("시뮬레이션 정지");
    entry.TimestampText = FLogEntry::FormatTimestamp();
    uiSubsystem->BroadcastOnLogEntryAdded(entry);

    uiSubsystem->BroadcastOnSimulationStateChanged(false);
}