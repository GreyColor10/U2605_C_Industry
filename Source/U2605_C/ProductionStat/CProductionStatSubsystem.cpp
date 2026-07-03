#include "ProductionStat/CProductionStatSubsystem.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"
#include "SimulationTime/CSimulationTimeSubsystem.h"
#include "ProductionStat/FProductionStatExporter.h"
#include "LogSender/FLogSender.h"
#include "ProductionStat/FDashboardDataBuilder.h"

void UCProductionStatSubsystem::ExportToCsv()
{
    ProductionStatExporter->ExportToCsv(CachedDashboardData, ProductCountByType);

    UWorld* world = GetWorld();
    CheckNotValid(world);

    FString logText = FString::Printf(TEXT("ProductionStat_%d.csv 저장 완료"),
        ProductionStatExporter->GetExportIndex());
    LogSender->SendLogMessage(world, ELogEventType::Info, logText);
    
    ProductionStatExporter->IncreaseExportIndex();
}

void UCProductionStatSubsystem::OnShortageScenarioStarted(const float InDuration)
{
    StartMeasurement(InDuration);

    UWorld* world = GetWorld();
    CheckNotValid(world);

    FString logText = FString::Printf(TEXT("전력 부족 시나리오(기간 %.1f초) 시작"), InDuration);
    LogSender->SendLogMessage(world, ELogEventType::Alert, logText);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->BroadcastOnScenarioActiveChanged(true, TEXT("전력 부족"));
}

void UCProductionStatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    ProductionStatExporter = MakePimpl<FProductionStatExporter>();
    ProductionStatExporter->InitializeExportIndex();

    LogSender = MakePimpl<FLogSender>();
    DashboardDataBuilder = MakePimpl<FDashboardDataBuilder>();

    UWorld* world = GetWorld();
    CheckNotValid(world);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->GetOnExportedDel().AddDynamic(this, &UCProductionStatSubsystem::ExportToCsv);
    uiSubsystem->GetOnShortageScenarioStart().AddDynamic(this, &UCProductionStatSubsystem::OnShortageScenarioStarted);
}

void UCProductionStatSubsystem::PostInitialize()
{
    Super::PostInitialize();

    UWorld* world = GetWorld();
    CheckNotValid(world);

    UCSimulationTimeSubsystem* timeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
    CheckNotValid(timeSubsystem);

    timeSubsystem->GetOnSimulationStateChangedDel().AddUObject(this, &UCProductionStatSubsystem::OnSimulationStateChanged);
}

void UCProductionStatSubsystem::Deinitialize()
{
    UWorld* world = GetWorld();
    if (IsValid(world))
    {
        UCSimulationTimeSubsystem* timeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
        if (IsValid(timeSubsystem))
            timeSubsystem->GetOnSimulationStateChangedDel().RemoveAll(this);

        UGameInstance* game = world->GetGameInstance();
        if (IsValid(game))
        {
            UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
            if (IsValid(uiSubsystem))
            {
                uiSubsystem->GetOnExportedDel().RemoveAll(this);
                uiSubsystem->GetOnShortageScenarioStart().RemoveAll(this);
            } 
        }
    }

    Super::Deinitialize();
}

void UCProductionStatSubsystem::ReceiveFinalProduct()
{
    StoredFinalProductNum++;
    SendDashboardData();
}

void UCProductionStatSubsystem::ReceiveIntermediateProduct(EProductType InType)
{
	ProductCountByType.FindOrAdd(InType)++;
}

void UCProductionStatSubsystem::RegisterEquipment(AActor* InEquipment)
{
    CheckNotValid(InEquipment);
    OperatingRecords.FindOrAdd(InEquipment);
}

void UCProductionStatSubsystem::NotifyEquipmentProcessingStateChanged(AActor* InEquipment, bool InIsProcessing)
{
    CheckNotValid(InEquipment);

    UWorld* world = GetWorld();
    CheckNotValid(world);

    UCSimulationTimeSubsystem* simTimeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
    CheckNotValid(simTimeSubsystem);

    float now = simTimeSubsystem->GetElapsedSeconds();
    FEquipmentOperatingRecord& record = OperatingRecords.FindOrAdd(InEquipment);

    if (InIsProcessing)
    {
        record.CurrentStartTime = now;
        return;
    }

    CheckTrue(record.CurrentStartTime < 0.0f);

    record.AccumulatedSeconds += now - record.CurrentStartTime;
    record.CurrentStartTime = -1.0f;
}

void UCProductionStatSubsystem::StartSendingDashboardData()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    FTimerDelegate del;
    del.BindUObject(this, &UCProductionStatSubsystem::SendDashboardData);
    world->GetTimerManager().SetTimer(DashboardHandle, del, 1.0f, true, 0.0f);
}

void UCProductionStatSubsystem::ResumeSendingDashboardData()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    world->GetTimerManager().UnPauseTimer(DashboardHandle);
}

void UCProductionStatSubsystem::PauseSendingDashboardData()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    world->GetTimerManager().PauseTimer(DashboardHandle);
}

void UCProductionStatSubsystem::SendDashboardData()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    CachedDashboardData = DashboardDataBuilder->BuildDashboardData(world, StoredFinalProductNum, OperatingRecords);
    DashboardHistory.Add(CachedDashboardData);

    uiSubsystem->BroadcastOnDashboardUpdated(CachedDashboardData);
}

void UCProductionStatSubsystem::OnSimulationStateChanged(bool InIsRunning)
{
    if (InIsRunning)
    {
        UWorld* world = GetWorld();
        CheckNotValid(world);

        if (world->GetTimerManager().IsTimerPaused(DashboardHandle)) ResumeSendingDashboardData();
        else StartSendingDashboardData();
    }

    else PauseSendingDashboardData();
}

void UCProductionStatSubsystem::StartMeasurement(const float InDuration)
{
    CheckTrue(bIsMeasuring);

    UWorld* world = GetWorld();
    CheckNotValid(world);

    UCSimulationTimeSubsystem* simTimeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
    CheckNotValid(simTimeSubsystem);

    ScenarioStartTime = simTimeSubsystem->GetElapsedSeconds();
    ScenarioEndTime = -1.0f;
    ScenarioDuration = InDuration;
    bIsMeasuring = true;

    FTimerDelegate del;
    del.BindUObject(this, &UCProductionStatSubsystem::EndMeasurement);
    world->GetTimerManager().SetTimer(ScenarioHandle, del, InDuration, false);

    StartScenarioRemainingTimer();
}

void UCProductionStatSubsystem::EndMeasurement()
{
    CheckFalse(bIsMeasuring);

    UWorld* world = GetWorld();
    CheckNotValid(world);

    UCSimulationTimeSubsystem* simTimeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
    CheckNotValid(simTimeSubsystem);

    ScenarioEndTime = simTimeSubsystem->GetElapsedSeconds();
    bIsMeasuring = false;

    FScenarioComparisonResult result = BuildComparisonResult();

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

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->BroadcastOnScenarioComparisonReady(CachedComparisonResult);
    uiSubsystem->BroadcastOnScenarioActiveChanged(false, FString());
}

void UCProductionStatSubsystem::StartScenarioRemainingTimer()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    FTimerDelegate del;
    del.BindUObject(this, &UCProductionStatSubsystem::ScenarioRemainingUpdated);
    world->GetTimerManager().SetTimer(ScenarioRemainingHandle, del, 0.1f, true, 0.0f);
}

void UCProductionStatSubsystem::StopScenarioRemainingTimer()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    world->GetTimerManager().ClearTimer(ScenarioRemainingHandle);
}

void UCProductionStatSubsystem::ScenarioRemainingUpdated()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->BroadcastOnScenarioRemainingUpdated(GetScenarioRemainingSeconds());
}

bool UCProductionStatSubsystem::FindProductionAtTime(float InTime, float& OutProduction) const
{
    CheckTrueResult(DashboardHistory.Num() == 0, false);
    CheckTrueResult(InTime < DashboardHistory[0].ElapsedSeconds, false);

    int32 low = 0;
    int32 high = DashboardHistory.Num() - 1;
    int32 found = 0;

    while (low <= high)
    {
        int32 mid = low + (high - low) / 2;

        if (DashboardHistory[mid].ElapsedSeconds <= InTime)
        {
            found = mid;
            low = mid + 1;
        }

        else high = mid - 1;
    }

    OutProduction = DashboardHistory[found].TotalCreamBread;
    return true;
}

FScenarioComparisonResult UCProductionStatSubsystem::BuildComparisonResult() const
{
    FScenarioComparisonResult result;

    float duration = ScenarioEndTime - ScenarioStartTime;
    CheckTrueResult(duration <= 0.0f, result);

    result.DurationSeconds = duration;

    float scenarioStartProd, scenarioEndProd;
    bool bScenarioOk =
        FindProductionAtTime(ScenarioStartTime, scenarioStartProd) &&
        FindProductionAtTime(ScenarioEndTime, scenarioEndProd);

    float normalStartProd, normalEndProd;
    bool bNormalOk =
        FindProductionAtTime(ScenarioStartTime - duration, normalStartProd) &&
        FindProductionAtTime(ScenarioStartTime, normalEndProd);

    CheckFalseResult(bScenarioOk && bNormalOk, result);

    result.ScenarioProduction = scenarioEndProd - scenarioStartProd;
    result.NormalProduction = normalEndProd - normalStartProd;

    result.ProductionChangePercent = (result.NormalProduction > 0.0f)
        ? ((result.ScenarioProduction - result.NormalProduction) / result.NormalProduction) * 100.0f
        : 0.0f;

    result.NormalThroughput = (duration > 0.0f) ? (result.NormalProduction / duration) * 60.0f : 0.0f;
    result.ScenarioThroughput = (duration > 0.0f) ? (result.ScenarioProduction / duration) * 60.0f : 0.0f;

    result.bIsValid = true;
    return result;
}

float UCProductionStatSubsystem::GetScenarioRemainingSeconds() const
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
