#include "Scenario/FScenarioResultExporter.h"
#include "Global.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

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

    FString directory = FPaths::ProjectSavedDir() + TEXT("ScenarioResult/");
    FString filePath = FString::Printf(TEXT("%sScenarioResult_%d.csv"), *directory, ExportIndex);

    FFileHelper::SaveStringToFile(csv, *filePath, FFileHelper::EEncodingOptions::ForceUTF8);

    int32 oldIndex = ExportIndex - 5;
    if (oldIndex > 0)
    {
        FString oldFilePath = FString::Printf(TEXT("%sScenarioResult_%d.csv"), *directory, oldIndex);
        if (FPaths::FileExists(oldFilePath))
            IFileManager::Get().Delete(*oldFilePath);
    }
}

void FScenarioResultExporter::InitializeExportIndex()
{
    FString directory = FPaths::ProjectSavedDir() + TEXT("ScenarioResult/");
    TArray<FString> foundFiles;
    IFileManager::Get().FindFiles(foundFiles, *(directory + TEXT("ScenarioResult_*.csv")), true, false);

    int32 maxIndex = 0;
    for (const FString& fileName : foundFiles)
    {
        FString indexStr = fileName.Replace(TEXT("ScenarioResult_"), TEXT(""));
        indexStr = indexStr.Replace(TEXT(".csv"), TEXT(""));

        int32 index = FCString::Atoi(*indexStr);
        if (index > maxIndex)
            maxIndex = index;
    }

    ExportIndex = maxIndex + 1;
}