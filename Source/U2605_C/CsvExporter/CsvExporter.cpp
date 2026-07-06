#include "CsvExporter/CsvExporter.h"
#include "Global.h"

CsvExporter::CsvExporter(const FString& InFolderName)
    : FolderName(InFolderName)
{
}

void CsvExporter::SaveToCsv(const FString& InCsv)
{
    CheckTrue(FolderName.IsEmpty());
    
    FString directory = FString::Printf(TEXT("%s%s/"), *FPaths::ProjectSavedDir(), *FolderName);
    FString filePath = FString::Printf(TEXT("%s%s_%d.csv"), *directory, *FolderName, ExportIndex);

    FFileHelper::SaveStringToFile(InCsv, *filePath, FFileHelper::EEncodingOptions::ForceUTF8);

    int32 oldIndex = ExportIndex - 5;
    if (oldIndex > 0)
    {
        FString oldFilePath = FString::Printf(TEXT("%s%s_%d.csv"), *directory, *FolderName, oldIndex);
        if (FPaths::FileExists(oldFilePath))
            IFileManager::Get().Delete(*oldFilePath);
    }

    ExportIndex++;
}

void CsvExporter::SetExportIndexByFolder()
{
    CheckTrue(FolderName.IsEmpty());

    FString directory = FString::Printf(TEXT("%s%s/"), *FPaths::ProjectSavedDir(), *FolderName);
    FString existFileName = FString::Printf(TEXT("%s%s_*.csv"), *directory, *FolderName);
    TArray<FString> foundFiles;
    
    IFileManager::Get().FindFiles(foundFiles, *existFileName, true, false);

    int32 maxIndex = 0;
    for (const FString& fileName : foundFiles)
    {
        FString forwardReplacePart = FString::Printf(TEXT("%s_"), *FolderName);
        FString indexStr = fileName.Replace(*forwardReplacePart, TEXT(""));
        indexStr = indexStr.Replace(TEXT(".csv"), TEXT(""));

        int32 index = FCString::Atoi(*indexStr);
        if (index > maxIndex)
            maxIndex = index;
    }

    ExportIndex = maxIndex + 1;
}
