#include "SimulationTime/CSimulationTimeSubsystem.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"

void UCSimulationTimeSubsystem::ChangeSimulationState()
{
    if (bIsRunning) StopSimulation();
    else StartSimulation();
}

void UCSimulationTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UWorld* world = GetWorld();
    CheckNull(world);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->GetOnSimulationStateUIChangedDel().AddDynamic(this, &UCSimulationTimeSubsystem::ChangeSimulationState);
}

void UCSimulationTimeSubsystem::Deinitialize()
{
    UWorld* world = GetWorld();
    if (IsValid(world))
    {
        UGameInstance* game = world->GetGameInstance();
        if (IsValid(game))
        {
            UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
            if (IsValid(uiSubsystem))
                uiSubsystem->GetOnSimulationStateUIChangedDel().RemoveAll(this);
        }
    }

    Super::Deinitialize();
}

float UCSimulationTimeSubsystem::GetElapsedSeconds() const
{
    CheckFalseResult(bIsRunning, PausedElapsedSeconds);

    UWorld* world = GetWorld();
    CheckNotValidResult(world, PausedElapsedSeconds);

    float elapsedSecond;
    if (bIsRunning) elapsedSecond = world->GetTimeSeconds() - SimulationStartTime;
    else elapsedSecond = PausedElapsedSeconds;

    return elapsedSecond;
}

void UCSimulationTimeSubsystem::StartSimulation()
{
    CheckTrue(bIsRunning);
    bIsRunning = true;

    UWorld* world = GetWorld();
    CheckNotValid(world);

    SimulationStartTime = world->GetTimeSeconds() - PausedElapsedSeconds;
    PausedElapsedSeconds = 0.0f;

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    FLogEntry entry;
    entry.EventType = ELogEventType::Info;
    entry.Message = TEXT("시뮬레이션 시작");
    entry.TimestampText = FLogEntry::FormatTimestamp();
    uiSubsystem->BroadcastOnLogEntryAdded(entry);

    BroadcastOnSimulationStateChanged(bIsRunning);
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

    BroadcastOnSimulationStateChanged(bIsRunning);
}

void UCSimulationTimeSubsystem::BroadcastOnSimulationStateChanged(bool InIsRunning)
{
    if (OnSimulationStateChanged.IsBound())
        OnSimulationStateChanged.Broadcast(InIsRunning);
}
