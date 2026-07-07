#pragma once

#include "CoreMinimal.h"

class U2605_C_API CsvExporter
{
public:
	CsvExporter(const FString& InFolderName);

public:
	void InitializeExportIndex();
	
	FORCEINLINE FString GetSavedLogText() const
	{
		return FString::Printf(TEXT("%s_%d.csv 저장 완료"), *FolderName, ExportIndex - 1);
	}

protected:
	void SaveToCsv(const FString& InCsv);

private:
	int32 ExportIndex = 1;
	const FString FolderName;
};
