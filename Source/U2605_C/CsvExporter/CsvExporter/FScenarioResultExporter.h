#pragma once

#include "CoreMinimal.h"
#include "StructData/CStructDatas.h"
#include "CsvExporter/CsvExporter.h"

class U2605_C_API FScenarioResultExporter : public CsvExporter
{
public:
	FScenarioResultExporter();

public:
	void ExportToCsv(const FScenarioComparisonResult& InResult) const;
	void InitializeExportIndex() override;
};