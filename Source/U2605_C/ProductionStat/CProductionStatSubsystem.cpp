#include "ProductionStat/CProductionStatSubsystem.h"
#include "Global.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

#include "Communication/CCommunicationSubsystem_UI.h"

void UCProductionStatSubsystem::ExportToCsv()
{
    UWorld* world = GetWorld();
    CheckNotValid(world);

    float elapsedSeconds = SimulationStartTime < 0.0f
        ? 0.0f
        : world->GetTimeSeconds() - SimulationStartTime;

    float throughputPerMinute = (elapsedSeconds > 0.0f)
        ? (StoredFinalProductNum / elapsedSeconds) * 60.0f
        : 0.0f;

    const UEnum* enumPtr = StaticEnum<EProductType>();

    FString csv;

    csv += TEXT("총 생산량,경과 시간(초),분당 생산량\n");
    csv += FString::Printf(TEXT("%d,%.2f,%.2f\n"), StoredFinalProductNum, elapsedSeconds, throughputPerMinute);
    csv += TEXT("\n");

    csv += TEXT("상품종류,생산량\n");
    for (const auto& pair : ProductCountByType)
    {
        FString typeName = enumPtr
            ? enumPtr->GetDisplayNameTextByValue((int64)pair.Key).ToString()
            : FString::FromInt((int32)pair.Key);

        csv += FString::Printf(TEXT("%s,%d\n"), *typeName, pair.Value);
    }

    FString directory = FPaths::ProjectSavedDir() + TEXT("ProductionStat/");
    FString filePath = FString::Printf(TEXT("%sProductionStat_%d.csv"), *directory, ExportIndex);

    FFileHelper::SaveStringToFile(csv, *filePath, FFileHelper::EEncodingOptions::ForceUTF8);

    FLog::Print(FString::Printf(TEXT("CSV 저장 완료: %s"), *filePath));

    int32 oldIndex = ExportIndex - 5;
    if (oldIndex > 0)
    {
        FString oldFilePath = FString::Printf(TEXT("%sProductionStat_%d.csv"), *directory, oldIndex);
        if (FPaths::FileExists(oldFilePath))
            IFileManager::Get().Delete(*oldFilePath);
    }

    ExportIndex++;
}

void UCProductionStatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

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

void UCProductionStatSubsystem::ReceiveFinalProduct()
{
	UWorld* world = GetWorld();
	CheckNotValid(world);

	if (SimulationStartTime < 0.0f)
		SimulationStartTime = world->GetTimeSeconds();

	StoredFinalProductNum++;

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	commuSubsystem_UI->BroadcastOnStoredFinalProductUpdated(StoredFinalProductNum);
}

void UCProductionStatSubsystem::ReceiveIntermediateProduct(EProductType InType)
{
	ProductCountByType.FindOrAdd(InType)++;
}


