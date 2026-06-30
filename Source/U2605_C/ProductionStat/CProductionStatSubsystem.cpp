#include "ProductionStat/CProductionStatSubsystem.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"
#include "SimulationTime/CSimulationTimeSubsystem.h"
#include "ProductionStat/FProductionStatExporter.h"
#include "LogSender/FLogSender.h"

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

void UCProductionStatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    ProductionStatExporter = MakePimpl<FProductionStatExporter>();
    ProductionStatExporter->InitializeExportIndex();

    LogSender = MakePimpl<FLogSender>();

    UWorld* world = GetWorld();
    CheckNotValid(world);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->GetOnExportedDel().AddDynamic(this, &UCProductionStatSubsystem::ExportToCsv);
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
                uiSubsystem->GetOnExportedDel().RemoveAll(this);
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

    CachedDashboardData = BuildDashboardData();
    DashboardHistory.Add(CachedDashboardData);

    uiSubsystem->BroadcastOnDashboardUpdated(CachedDashboardData);
}

FDashboardData UCProductionStatSubsystem::BuildDashboardData() const
{
    FDashboardData data;
    data.TotalCreamBread = StoredFinalProductNum;

    UWorld* world = GetWorld();
    CheckNotValidResult(world, data);

    UCSimulationTimeSubsystem* simTimeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
    CheckNotValidResult(simTimeSubsystem, data);

    float elapsed = simTimeSubsystem->GetElapsedSeconds();
    data.ElapsedSeconds = FMath::Max(elapsed, 0.0f);
    data.ThroughputPerMinute = (elapsed > 0.0f)
        ? (StoredFinalProductNum / elapsed) * 60.0f
        : 0.0f;

    float rateSum = 0.0f;
    int32 equipCount = 0;

    for (const auto& pair : OperatingRecords)
    {
        if (!pair.Key.IsValid()) continue;

        float operatingSeconds = pair.Value.AccumulatedSeconds;

        if (pair.Value.CurrentStartTime >= 0.0f)
            operatingSeconds += elapsed - pair.Value.CurrentStartTime;

        rateSum += (elapsed > 0.0f) ? (operatingSeconds / elapsed) : 0.0f;
        equipCount++;
    }

    data.OperatingRate = (equipCount > 0) ? (rateSum / equipCount) : 0.0f;

    return data; 
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

void UCProductionStatSubsystem::StartMeasurement()
{
    CheckTrue(bIsMeasuring);

    UWorld* world = GetWorld();
    CheckNotValid(world);

    UCSimulationTimeSubsystem* simTimeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
    CheckNotValid(simTimeSubsystem);

    ScenarioStartTime = simTimeSubsystem->GetElapsedSeconds();
    ScenarioEndTime = -1.0f;
    bIsMeasuring = true;

    FLog::Print(FString::Printf(TEXT("측정 시작: %.1f초"), ScenarioStartTime));
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
        FLog::Print(FString::Printf(TEXT("정상 %.0f개 vs 시나리오 %.0f개 (%.1f%%)"),
            result.NormalProduction, result.ScenarioProduction, result.ProductionChangePercent));
    }
    else
    {
        FLog::Print(TEXT("비교 불가: 직전 정상 구간 데이터 부족"));
    }

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->BroadcastOnScenarioComparisonReady(CachedComparisonResult);
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
