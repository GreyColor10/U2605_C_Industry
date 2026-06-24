#pragma once

#include "CoreMinimal.h"
#include "StructData/CStructDatas.h"

class U2605_C_API FProductionStatExporter
{
public:
	void ExportToCsv(FDashboardData& InDashboardData, TMap<EProductType, int32>& InProductCountByType) const;
	void InitializeExportIndex();

	FORCEINLINE const int32 GetExportIndex() const { return ExportIndex; };
	FORCEINLINE void IncreaseExportIndex() { ExportIndex++; };

private:
	int32 ExportIndex = 1;
};
