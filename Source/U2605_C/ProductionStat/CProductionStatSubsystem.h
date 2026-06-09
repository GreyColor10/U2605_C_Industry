#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "StructData/CStructDatas.h"
#include "CProductionStatSubsystem.generated.h"

UCLASS()
class U2605_C_API UCProductionStatSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ProductionStat")
	void ExportToCsv();

	UFUNCTION(BlueprintCallable, Category = "ProductionStat")
	void StartSimulation();

	UFUNCTION(BlueprintCallable, Category = "ProductionStat")
	void StopSimulation();

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

public:
	void ReceiveFinalProduct();
	void ReceiveIntermediateProduct(EProductType InType);

	FORCEINLINE bool IsRunning() const { return bIsRunning; }
	FORCEINLINE float GetSimulationStartTime() const { return SimulationStartTime; }

private:
	void OnDashboardTick();
	FDashboardData BuildDashboardData() const;

	void OnSimulationStateChanged(bool InIsRunning);

private:
	int StoredFinalProductNum = 0;
	TMap<EProductType, int32> ProductCountByType;        
	int32 ExportIndex = 1;

	bool bIsRunning = false;
	float SimulationStartTime = -1.0f;
	float PausedElapsedSeconds = 0.0f;

	FTimerHandle DashboardHandle;
};
