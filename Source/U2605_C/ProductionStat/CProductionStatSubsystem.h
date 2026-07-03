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

	UFUNCTION()
	void OnShortageScenarioStarted(const float InDuration);

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
	void StartMeasurement(const float InDuration);
	void EndMeasurement();

	void StartScenarioRemainingTimer();
	void StopScenarioRemainingTimer();
	void ScenarioRemainingUpdated();

	bool FindProductionAtTime(float InTime, float& OutProduction) const;
	FScenarioComparisonResult BuildComparisonResult() const;

	float GetScenarioRemainingSeconds() const;

private:
	TPimplPtr<class FProductionStatExporter> ProductionStatExporter;
	TPimplPtr<class FLogSender> LogSender;
	TPimplPtr<class FDashboardDataBuilder> DashboardDataBuilder;

private:
	int StoredFinalProductNum = 0;
	TMap<EProductType, int32> ProductCountByType;        
	TMap<TWeakObjectPtr<AActor>, FEquipmentOperatingRecord> OperatingRecords;

	FTimerHandle DashboardHandle;
	FTimerHandle ScenarioHandle;
	FTimerHandle ScenarioRemainingHandle;

	FDashboardData CachedDashboardData;
	TArray<FDashboardData> DashboardHistory;

	bool bIsMeasuring = false;
	float ScenarioStartTime = -1.0f;
	float ScenarioEndTime = -1.0f;
	float ScenarioDuration = -1.0f;

	FScenarioComparisonResult CachedComparisonResult;
};
