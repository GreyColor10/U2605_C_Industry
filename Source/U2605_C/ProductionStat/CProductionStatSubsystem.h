#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "StructData/CStructDatas.h"
#include "Templates/PimplPtr.h"
#include "CProductionStatSubsystem.generated.h"

struct FEquipmentOperatingRecord
{
	float AccumulatedSeconds = 0.0f;
	float CurrentStartTime = -1.0f;
};

UCLASS()
class U2605_C_API UCProductionStatSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	UFUNCTION(BlueprintCallable, Category = "ProductionStat")
	void ExportToCsv();

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

public:
	void ReceiveFinalProduct();
	void ReceiveIntermediateProduct(EProductType InType);

	void RegisterEquipment(AActor* InEquipment);
	void NotifyEquipmentProcessingStateChanged(AActor* InEquipment, bool InIsProcessing);

private:
	void StartSendingDashboardData();
	void ResumeSendingDashboardData();
	void PauseSendingDashboardData();

private:
	void SendDashboardData();
	FDashboardData BuildDashboardData() const;

	void OnSimulationStateChanged(bool InIsRunning);

private:
	TPimplPtr<class FProductionStatExporter> ProductionStatExporter;

private:
	int StoredFinalProductNum = 0;
	TMap<EProductType, int32> ProductCountByType;        
	TMap<TWeakObjectPtr<AActor>, FEquipmentOperatingRecord> OperatingRecords;

	FTimerHandle DashboardHandle;
	
};
