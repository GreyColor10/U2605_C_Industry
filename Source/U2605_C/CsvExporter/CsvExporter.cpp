#include "CsvExporter/CsvExporter.h"
#include "Global.h"

void CsvExporter::SaveToCsv(const FString& InFolderName, const FString& InCsv) const
{
    FString directory = FPaths::ProjectSavedDir() + InFolderName + TEXT("/");
    FString filePath = FString::Printf(TEXT("%s%s_%d.csv"), *directory, *InFolderName, ExportIndex);

    FFileHelper::SaveStringToFile(InCsv, *filePath, FFileHelper::EEncodingOptions::ForceUTF8);

    int32 oldIndex = ExportIndex - 5;
    if (oldIndex > 0)
    {
        FString oldFilePath = FString::Printf(TEXT("%s%s_%d.csv"), *directory, *InFolderName, oldIndex);
        if (FPaths::FileExists(oldFilePath))
            IFileManager::Get().Delete(*oldFilePath);
    }
}

void CsvExporter::SetExportIndexByFolder(const FString& InFolderName)
{
    FString directory = FPaths::ProjectSavedDir() + InFolderName + TEXT("/");
    TArray<FString> foundFiles;
    IFileManager::Get().FindFiles(foundFiles, *(directory + InFolderName + TEXT("_*.csv")), true, false);

    int32 maxIndex = 0;
    for (const FString& fileName : foundFiles)
    {
        FString indexStr = fileName.Replace(*(InFolderName + TEXT("_")), TEXT(""));
        indexStr = indexStr.Replace(TEXT(".csv"), TEXT(""));

        int32 index = FCString::Atoi(*indexStr);
        if (index > maxIndex)
            maxIndex = index;
    }

    ExportIndex = maxIndex + 1;
}
