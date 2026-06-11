#include "ProductionStat/FProductionStatExporter.h"
#include "Global.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

void FProductionStatExporter::ExportToCsv(float InElapsedSeconds, int InStoredFinalProductNum, TMap<EProductType, int32>& InProductCountByType) const
{
    float throughputPerMinute = (InElapsedSeconds > 0.0f)
        ? (InStoredFinalProductNum / InElapsedSeconds) * 60.0f
        : 0.0f;

    const UEnum* enumPtr = StaticEnum<EProductType>();

    FString csv;

    csv += TEXT("총 생산량,경과 시간(초),분당 생산량\n");
    csv += FString::Printf(TEXT("%d,%.2f,%.2f\n"), InStoredFinalProductNum, InElapsedSeconds, throughputPerMinute);
    csv += TEXT("\n");

    csv += TEXT("상품종류,생산량\n");
    for (const auto& pair : InProductCountByType)
    {
        FString typeName = enumPtr
            ? enumPtr->GetDisplayNameTextByValue((int64)pair.Key).ToString()
            : FString::FromInt((int32)pair.Key);

        csv += FString::Printf(TEXT("%s,%d\n"), *typeName, pair.Value);
    }

    FString directory = FPaths::ProjectSavedDir() + TEXT("ProductionStat/");
    FString filePath = FString::Printf(TEXT("%sProductionStat_%d.csv"), *directory, ExportIndex);

    FFileHelper::SaveStringToFile(csv, *filePath, FFileHelper::EEncodingOptions::ForceUTF8);

    int32 oldIndex = ExportIndex - 5;
    if (oldIndex > 0)
    {
        FString oldFilePath = FString::Printf(TEXT("%sProductionStat_%d.csv"), *directory, oldIndex);
        if (FPaths::FileExists(oldFilePath))
            IFileManager::Get().Delete(*oldFilePath);
    }
}

void FProductionStatExporter::InitializeExportIndex()
{
    FString directory = FPaths::ProjectSavedDir() + TEXT("ProductionStat/");
    TArray<FString> foundFiles;
    IFileManager::Get().FindFiles(foundFiles, *(directory + TEXT("ProductionStat_*.csv")), true, false);

    int32 maxIndex = 0;
    for (const FString& fileName : foundFiles)
    {
        FString indexStr = fileName.Replace(TEXT("ProductionStat_"), TEXT(""));
        indexStr = indexStr.Replace(TEXT(".csv"), TEXT(""));

        int32 index = FCString::Atoi(*indexStr);
        if (index > maxIndex)
            maxIndex = index;
    }

    ExportIndex = maxIndex + 1;
}
