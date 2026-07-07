#include "CsvExporter/CsvExporter/FProductionStatExporter.h"
#include "Global.h"

FProductionStatExporter::FProductionStatExporter()
    : CsvExporter(TEXT("ProductionStat"))
{

}

void FProductionStatExporter::ExportToCsv(const FDashboardData& InDashboardData, const TMap<EProductType, int32>& InProductCountByType)
{
    const UEnum* enumPtr = StaticEnum<EProductType>();

    FString csv;

    csv += TEXT("총 생산량,경과 시간,분당 처리량,설비 가동률(%)\n");

    int32 totalSeconds = FMath::FloorToInt(InDashboardData.ElapsedSeconds);
    int32 minutes = totalSeconds / 60;
    int32 seconds = totalSeconds % 60;
    FString elapsedStr = FString::Printf(TEXT("%02d:%02d"), minutes, seconds);

    csv += FString::Printf(TEXT("%d,%s,%.1f,%.1f\n"), 
        InDashboardData.TotalCreamBread, 
        *elapsedStr, 
        InDashboardData.ThroughputPerMinute,
        InDashboardData.OperatingRate * 100.0f
    );

    csv += TEXT("상품 종류,생산량\n");
    for (const auto& pair : InProductCountByType)
    {
        FString typeName = enumPtr
            ? enumPtr->GetDisplayNameTextByValue((int64)pair.Key).ToString()
            : FString::FromInt((int32)pair.Key);

        csv += FString::Printf(TEXT("%s,%d\n"), *typeName, pair.Value);
    }

    SaveToCsv(csv);
}
