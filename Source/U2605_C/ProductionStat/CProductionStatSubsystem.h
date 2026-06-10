#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "StructData/CStructDatas.h"
#include "CProductionStatSubsystem.generated.h"

UCLASS()
class U2605_C_API UCProductionStatSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	UFUNCTION(BlueprintCallable, Category = "ProductionStat")
	void ExportToCsv();

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	/*void Deinitialize() override;*/

public:
	void ReceiveFinalProduct();
	void ReceiveIntermediateProduct(EProductType InType);

private:
	void OnDashboardTick();
	FDashboardData BuildDashboardData() const;

private:
	int StoredFinalProductNum = 0;
	TMap<EProductType, int32> ProductCountByType;        
	int32 ExportIndex = 1;

	FTimerHandle DashboardHandle;
};
