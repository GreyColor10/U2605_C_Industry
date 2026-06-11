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

    FLogEntry entry;
    entry.EventType = ELogEventType::Info;
    entry.Message = TEXT("[INFO] 시뮬레이션 시작");
    entry.Timestamp = world->GetTimeSeconds() - SimulationStartTime;
    entry.TimestampText = FLogEntry::FormatTimestamp(entry.Timestamp);
    commuSubsystem_UI->BroadcastOnLogEntryAdded(entry);

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

    FLogEntry entry;
    entry.EventType = ELogEventType::Info;
    entry.Message = TEXT("[INFO] 시뮬레이션 정지");
    entry.Timestamp = PausedElapsedSeconds;
    entry.TimestampText = FLogEntry::FormatTimestamp(entry.Timestamp);
    commuSubsystem_UI->BroadcastOnLogEntryAdded(entry);

    commuSubsystem_UI->BroadcastOnSimulationStateChanged(false);
}