#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/CFacilityType.h"
#include "CProductionStatSubsystem.generated.h"

UCLASS()
class U2605_C_API UCProductionStatSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ProductionStat")
	void ExportToCsv();

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void ReceiveFinalProduct();
	void ReceiveIntermediateProduct(EProductType InType);

private:
	int StoredFinalProductNum = 0;
	TMap<EProductType, int32> ProductCountByType;        
	float SimulationStartTime = -1.0f; 
	int32 ExportIndex = 1;
};
