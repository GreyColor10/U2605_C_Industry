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
	UFUNCTION()
	void ExportToCsv();

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void PostInitialize() override;
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
	void SendDashboardData();

private:
	void OnSimulationStateChanged(bool InIsRunning);

public:
	FScenarioComparisonResult BuildComparisonResult(float InScenarioStartTime, float InDuration) const;

private:
	bool FindProductionAtTime(float InTime, float& OutProduction) const;

private:
	TPimplPtr<class FProductionStatExporter> ProductionStatExporter;
	TPimplPtr<class FLogSender> LogSender;
	TPimplPtr<class FDashboardDataBuilder> DashboardDataBuilder;

private:
	FTimerHandle DashboardHandle;

private:
	int StoredFinalProductNum = 0;
	TMap<EProductType, int32> ProductCountByType;        
	TMap<TWeakObjectPtr<AActor>, FEquipmentOperatingRecord> OperatingRecords;

	FDashboardData CachedDashboardData;
	TArray<FDashboardData> DashboardHistory;
};
