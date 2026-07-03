#include "ProductionStat/FDashboardDataBuilder.h"
#include "Global.h"

#include "SimulationTime/CSimulationTimeSubsystem.h"

FDashboardData FDashboardDataBuilder::BuildDashboardData(UWorld* InWorld, int InStoredFinalProduct, const TMap<TWeakObjectPtr<AActor>, FEquipmentOperatingRecord>& InOperatingRecords) const
{
    FDashboardData data;
    data.TotalCreamBread = InStoredFinalProduct;

    CheckNotValidResult(InWorld, data);

    UCSimulationTimeSubsystem* simTimeSubsystem = InWorld->GetSubsystem<UCSimulationTimeSubsystem>();
    CheckNotValidResult(simTimeSubsystem, data);

    float elapsed = simTimeSubsystem->GetElapsedSeconds();
    data.ElapsedSeconds = FMath::Max(elapsed, 0.0f);
    data.ThroughputPerMinute = (elapsed > 0.0f)
        ? (InStoredFinalProduct / elapsed) * 60.0f
        : 0.0f;

    float rateSum = 0.0f;
    int32 equipCount = 0;

    for (const auto& pair : InOperatingRecords)
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