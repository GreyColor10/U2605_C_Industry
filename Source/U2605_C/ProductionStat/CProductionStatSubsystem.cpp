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

    ProductionStatExporter->ExportToCsv(simTimeSubsystem->GetElapsedSeconds(), StoredFinalProductNum, ProductCountByType);

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(commuSubsystem_UI);

    FLogEntry entry;
    entry.EventType = ELogEventType::Info;
    entry.Message = FString::Printf(TEXT("[INFO] ProductionStat_%d.csv 저장 완료"), 
        ProductionStatExporter->GetExportIndex());
    entry.TimestampText = FLogEntry::FormatTimestamp();

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

    UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(commuSubsystem_UI);

    commuSubsystem_UI->BroadcastOnDashboardUpdated(BuildDashboardData());
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