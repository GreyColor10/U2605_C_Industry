#include "CScenarioSubsystem.h"
#include "Global.h"

#include "LogSender/FLogSender.h"
#include "SimulationTime/CSimulationTimeSubsystem.h"
#include "Communication/CCommunicationSubsystem_UI.h"
#include "ProductionStat/CProductionStatSubsystem.h"

void UCScenarioSubsystem::OnShortageScenarioStarted(const float InDuration)
{
    StartMeasurement(InDuration);

    UWorld* world = GetWorld();
    CheckNotValid(world);

    FString logText = FString::Printf(TEXT("전력 부족 시나리오(기간 %.1f초) 시작"), InDuration);
    LogSender->SendLogMessage(world, ELogEventType::Alert, logText);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    if (OnShortageScenarioActiveChanged.IsBound())
        OnShortageScenarioActiveChanged.Broadcast(true);
}

void UCScenarioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LogSender = MakePimpl<FLogSender>();

    UWorld* world = GetWorld();
    CheckNotValid(world);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->GetOnShortageScenarioStart().AddDynamic(this, &UCScenarioSubsystem::OnShortageScenarioStarted);
}

void UCScenarioSubsystem::PostInitialize()
{
    Super::PostInitialize();

    UWorld* world = GetWorld();
    CheckNotValid(world);

    UCSimulationTimeSubsystem* timeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
    CheckNotValid(timeSubsystem);

    timeSubsystem->GetOnSimulationStateChangedDel().AddUObject(this, &UCScenarioSubsystem::OnSimulationStateChanged);
}

void UCScenarioSubsystem::Deinitialize()
{
    UWorld* world = GetWorld();
    if (IsValid(world))
    {
        UGameInstance* game = world->GetGameInstance();
        if (IsValid(game))
        {
            UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
            if (IsValid(uiSubsystem))
                uiSubsystem->GetOnShortageScenarioStart().RemoveAll(this);
        }
    }

    Super::Deinitialize();
}

void UCScenarioSubsystem::StartMeasurement(const float InDuration)
{
    CheckTrue(bIsMeasuring);

    UWorld* world = GetWorld();
    CheckNotValid(world);

    UCSimulationTimeSubsystem* simTimeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
    CheckNotValid(simTimeSubsystem);

    ScenarioStartTime = simTimeSubsystem->GetElapsedSeconds();
    ScenarioDuration = InDuration;
    bIsMeasuring = true;

    FTimerDelegate del;
    del.BindUObject(this, &UCScenarioSubsystem::EndMeasurement);
    world->GetTimerManager().SetTimer(ScenarioHandle, del, InDuration, false);

    StartScenarioRemainingTimer();
}

void UCScenarioSubsystem::EndMeasurement()
{
    CheckFalse(bIsMeasuring);

    UWorld* world = GetWorld();
    CheckNotValid(world);

    bIsMeasuring = false;
    FScenarioComparisonResult result;

    UCProductionStatSubsystem* proStatSubsystem = world->GetSubsystem<UCProductionStatSubsystem>();
    if(IsValid(proStatSubsystem))
        result = proStatSubsystem->BuildComparisonResult(ScenarioStartTime, ScenarioDuration);

    if (result.bIsValid)
    {
        /*FLog::Print(FString::Printf(TEXT("정상 %.0f개 vs 시나리오 %.0f개 (%.1f%%)"),
            result.NormalProduction, result.ScenarioProduction, result.ProductionChangePercent));*/

        FString logText = FString::Printf(TEXT("시나리오 종료"));
        LogSender->SendLogMessage(world, ELogEventType::Alert, logText);
    }
    else
    {
        FLog::Print(TEXT("비교 불가: 직전 정상 구간 데이터 부족"));
    }

    StopScenarioRemainingTimer();

    if (OnShortageScenarioActiveChanged.IsBound())
        OnShortageScenarioActiveChanged.Broadcast(false);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->BroadcastOnScenarioDeactived();
    uiSubsystem->BroadcastOnScenarioComparisonReady(result);
}

void UCScenarioSubsystem::StartScenarioRemainingTimer()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    FTimerDelegate del;
    del.BindUObject(this, &UCScenarioSubsystem::ScenarioRemainingUpdated);
    world->GetTimerManager().SetTimer(ScenarioRemainingHandle, del, 0.1f, true, 0.0f);
}

void UCScenarioSubsystem::StopScenarioRemainingTimer()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    world->GetTimerManager().ClearTimer(ScenarioRemainingHandle);
}

void UCScenarioSubsystem::ScenarioRemainingUpdated()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->BroadcastOnScenarioRemainingUpdated(GetScenarioRemainingSeconds());
}

float UCScenarioSubsystem::GetScenarioRemainingSeconds() const
{
    CheckFalseResult(bIsMeasuring, 0.0f);

    UWorld* world = GetWorld();
    CheckNotValidResult(world, 0.0f);

    UCSimulationTimeSubsystem* simTimeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
    CheckNotValidResult(simTimeSubsystem, 0.0f);

    float elapsed = simTimeSubsystem->GetElapsedSeconds();
    float remaining = ScenarioDuration - (elapsed - ScenarioStartTime);

    return FMath::Max(remaining, 0.0f);
}

void UCScenarioSubsystem::ResumeScenario()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    world->GetTimerManager().UnPauseTimer(ScenarioHandle);
    world->GetTimerManager().UnPauseTimer(ScenarioRemainingHandle);
}

void UCScenarioSubsystem::PauseScenario()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    world->GetTimerManager().PauseTimer(ScenarioHandle);
    world->GetTimerManager().PauseTimer(ScenarioRemainingHandle);
}

void UCScenarioSubsystem::OnSimulationStateChanged(bool InIsRunning)
{
    if (bIsMeasuring)
    {
        if (InIsRunning) ResumeScenario();
        else PauseScenario();
    }
}