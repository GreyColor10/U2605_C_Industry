#include "ProductionStat/CProductionStatSubsystem.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"
#include "SimulationTime/CSimulationTimeSubsystem.h"
#include "ProductionStat/FProductionStatExporter.h"

void UCProductionStatSubsystem::ExportToCsv()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    UCSimulationTimeSubsystem* simTimeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
    CheckNotValid(simTimeSubsystem);

    float simStartTime = simTimeSubsystem->GetSimulationStartTime();
    float elapsedSeconds = simStartTime < 0.0f
        ? 0.0f
        : world->GetTimeSeconds() - simStartTime;

    ProductionStatExporter->ExportToCsv(elapsedSeconds, StoredFinalProductNum, ProductCountByType);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(commuSubsystem_UI);

    FLogEntry entry;
    entry.EventType = ELogEventType::Info;
    entry.Message = FString::Printf(TEXT("[INFO] ProductionStat_%d.csv 저장 완료"), 
        ProductionStatExporter->GetExportIndex());
    entry.Timestamp = elapsedSeconds;
    entry.TimestampText = FLogEntry::FormatTimestamp(entry.Timestamp);

    commuSubsystem_UI->BroadcastOnLogEntryAdded(entry);

    ProductionStatExporter->IncreaseExportIndex();
}

void UCProductionStatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    ProductionStatExporter = MakePimpl<FProductionStatExporter>();
    ProductionStatExporter->InitializeExportIndex();

    UWorld* world = GetWorld();
    CheckNull(world);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(commuSubsystem_UI);

    commuSubsystem_UI->GetOnSimulationStateChangedDel().AddUObject(this, &UCProductionStatSubsystem::OnSimulationStateChanged);
}

void UCProductionStatSubsystem::Deinitialize()
{
    UWorld* world = GetWorld();
    if (IsValid(world))
    {
        if (UGameInstance* game = world->GetGameInstance())
        {
            if (UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>())
                commuSubsystem_UI->GetOnSimulationStateChangedDel().RemoveAll(this);
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

    UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(commuSubsystem_UI);

    commuSubsystem_UI->BroadcastOnDashboardUpdated(BuildDashboardData());
}

FDashboardData UCProductionStatSubsystem::BuildDashboardData() const
{
    FDashboardData data;
    data.TotalCreamBread = StoredFinalProductNum;
    data.ProductCountByType = ProductCountByType;

    UWorld* world = GetWorld();
    CheckNotValidResult(world, data);

    UCSimulationTimeSubsystem* simTimeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
    CheckNotValidResult(simTimeSubsystem, data);

    float elapsed = world->GetTimeSeconds() - simTimeSubsystem->GetSimulationStartTime();
    data.ElapsedSeconds = FMath::Max(elapsed, 0.0f);
    data.ThroughputPerMinute = (elapsed > 0.0f)
        ? (StoredFinalProductNum / elapsed) * 60.0f
        : 0.0f;

    data.OperatingRate = 0.0f;

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