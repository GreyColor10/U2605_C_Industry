#pragma once

#include "CoreMinimal.h"
#include "StructData/CStructDatas.h"

class U2605_C_API FDashboardDataBuilder
{
public:
	FDashboardData BuildDashboardData(UWorld* InWorld, int InStoredFinalProduct, const TMap<TWeakObjectPtr<AActor>, FEquipmentOperatingRecord>& InOperatingRecords) const;
};
