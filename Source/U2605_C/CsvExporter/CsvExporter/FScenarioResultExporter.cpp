#include "CsvExporter/CsvExporter/FScenarioResultExporter.h"
#include "Global.h"

FScenarioResultExporter::FScenarioResultExporter()
{
    FolderName = TEXT("ScenarioResult");
}

void FScenarioResultExporter::ExportToCsv(const FScenarioComparisonResult& InResult) const
{
    CheckFalse(InResult.bIsValid);

    FString csv;

    csv += TEXT("시나리오,지속 시간(초)\n");

    csv += FString::Printf(TEXT("전력 부족,%.1f\n\n"),
        InResult.DurationSeconds
    );

    csv += TEXT("상태,생산량,처리량(개/분)\n");

    csv += FString::Printf(TEXT("정상,%d,%.1f\n"),
        InResult.NormalProduction,
        InResult.NormalThroughput
    );

    csv += FString::Printf(TEXT("시나리오,%d,%.1f\n\n"),
        InResult.ScenarioProduction,
        InResult.ScenarioThroughput
    );

    csv += FString::Printf(TEXT("변화율(%%),%.1f\n"),
        InResult.ProductionChangePercent
    );

    SaveToCsv(csv);
}

void FScenarioResultExporter::InitializeExportIndex()
{
    SetExportIndexByFolder();
}