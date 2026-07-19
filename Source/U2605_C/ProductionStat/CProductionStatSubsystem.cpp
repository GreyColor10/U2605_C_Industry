#include "ProductionStat/CProductionStatSubsystem.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"
#include "SimulationTime/CSimulationTimeSubsystem.h"
#include "CsvExporter/CsvExporter/FProductionStatExporter.h"
#include "LogSender/FLogSender.h"
#include "ProductionStat/FDashboardDataBuilder.h"

void UCProductionStatSubsystem::ExportToCsv()
{
    ProductionStatExporter->ExportToCsv(CachedDashboardData, ProductCountByType);

    UWorld* world = GetWorld();
    CheckNotValid(world);

    LogSender->SendLogMessage(world, ELogEventType::Info, ProductionStatExporter->GetSavedLogText());
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

    uiSubsystem->GetOnProductionStatExportedDel().AddDynamic(this, &UCProductionStatSubsystem::ExportToCsv);
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
                uiSubsystem->GetOnProductionStatExportedDel().RemoveAll(this);
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

FScenarioComparisonResult UCProductionStatSubsystem::BuildComparisonResult(float InScenarioStartTime, float InDuration) const
{
    FScenarioComparisonResult result;

    CheckTrueResult(InDuration < 0.0f, result);
    CheckTrueResult(FMath::IsNearlyZero(InDuration), result);

    float scenarioEndTime = InScenarioStartTime + InDuration;
    result.DurationSeconds = InDuration;

    int32 scenarioStartProd, scenarioEndProd;
    bool bScenarioOk =
        FindProductionAtTime(InScenarioStartTime, scenarioStartProd) &&
        FindProductionAtTime(scenarioEndTime, scenarioEndProd);

    int32 normalStartProd, normalEndProd;
    bool bNormalOk =
        FindProductionAtTime(InScenarioStartTime - InDuration, normalStartProd) &&
        FindProductionAtTime(InScenarioStartTime, normalEndProd);

    CheckFalseResult(bScenarioOk && bNormalOk, result);

    result.ScenarioProduction = scenarioEndProd - scenarioStartProd;
    result.NormalProduction = normalEndProd - normalStartProd;

    result.ProductionChangePercent = (result.NormalProduction > 0)
        ? ((result.ScenarioProduction - result.NormalProduction) / (float)result.NormalProduction) * 100.0f
        : 0.0f;

    result.NormalThroughput = result.NormalProduction / InDuration * 60.0f;
    result.ScenarioThroughput = result.ScenarioProduction / InDuration * 60.0f;

    result.bIsValid = true;
    return result;
}

bool UCProductionStatSubsystem::FindProductionAtTime(float InTime, int32& OutProduction) const
{
    CheckTrueResult(DashboardHistory.IsEmpty(), false);
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


