#pragma once

#include "CoreMinimal.h"

class U2605_C_API CsvExporter
{
public:
	virtual void InitializeExportIndex() PURE_VIRTUAL(InitializeExportIndex,);
	
	FORCEINLINE const int32 GetExportIndex() const { return ExportIndex; };
	FORCEINLINE void IncreaseExportIndex() { ExportIndex++; };

protected:
	void SaveToCsv(const FString& InCsv) const;
	void SetExportIndexByFolder();

protected:
	int32 ExportIndex = 1;
	FString FolderName;
};
