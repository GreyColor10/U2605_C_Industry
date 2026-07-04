#pragma once

#include "CoreMinimal.h"
#include "StructData/CStructDatas.h"

class U2605_C_API FScenarioResultExporter
{
public:
	void ExportToCsv(const FScenarioComparisonResult& InResult) const;
	void InitializeExportIndex();

	FORCEINLINE const int32 GetExportIndex() const { return ExportIndex; };
	FORCEINLINE void IncreaseExportIndex() { ExportIndex++; };

private:
	int32 ExportIndex = 1;
};