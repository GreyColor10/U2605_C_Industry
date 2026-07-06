#pragma once

#include "CoreMinimal.h"
#include "StructData/CStructDatas.h"
#include "CsvExporter/CsvExporter.h"

class U2605_C_API FProductionStatExporter : public CsvExporter
{
public:
	FProductionStatExporter();

public:
	void ExportToCsv(const FDashboardData& InDashboardData, const TMap<EProductType, int32>& InProductCountByType) const;
	void InitializeExportIndex() override;
};
