#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StructData/CStructDatas.h"
#include "CCommunicationSubsystem_UI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStoredFinalProductUpdated, int, InStoredFinalProductNum);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStorageInfoUpdated, const FStorageInfoData&, InStorageInfoData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProcessorInfoUpdated, const FProcessorInfoData&, InProcessorInfoData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDashboardUpdated, const FDashboardData&, InDashboardData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLogEntryAdded, const FLogEntry&, InLogEntry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProcessorProgressUpdated, float, InProgress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScenarioComparisonReady, const FScenarioComparisonResult&, InResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScenarioRemainingUpdated, float, InRemainingSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProductionStatExported);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimulationStateUIChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScenarioDeactived);

DECLARE_DELEGATE_OneParam(FUITargetChanged, AActor*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FProcessingTimeChangeRequested, UClass*, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FShortageScenarioStarted, const float);
DECLARE_MULTICAST_DELEGATE(FProcessingTimeChangeEnded);
DECLARE_MULTICAST_DELEGATE(FScenarioResultExported);

DECLARE_DELEGATE_RetVal(const AActor*, FUITargetGotten)

UCLASS()
class U2605_C_API UCCommunicationSubsystem_UI : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	FStorageInfoUpdated OnStorageInfoUpdated;

	UPROPERTY()
	FProcessorInfoUpdated OnProcessorInfoUpdated;

	UPROPERTY()
	FDashboardUpdated OnDashboardUpdated;

	UPROPERTY()
	FLogEntryAdded OnLogEntryAdded;

	UPROPERTY()
	FProcessorProgressUpdated OnProcessorProgressUpdated;

	UPROPERTY()
	FScenarioComparisonReady OnScenarioComparisonReady;

	UPROPERTY()
	FScenarioRemainingUpdated OnScenarioRemainingUpdated;

	UPROPERTY()
	FProductionStatExported OnProductionStatExported;

	UPROPERTY()
	FSimulationStateUIChanged OnSimulationStateUIChanged;

	UPROPERTY()
	FScenarioDeactived OnScenarioDeactived;

private:
	UFUNCTION(BlueprintCallable, Category = "Export")
	void BroadcastOnProductionStatExported();

	UFUNCTION(BlueprintCallable, Category = "SimulationState")
	void BroadcastOnSimulationStateUIChanged();

public:
	void BroadcastOnStorageInfoUpdated(const FStorageInfoData& InStorageInfoData);
	void BroadcastOnProcessorInfoUpdated(const FProcessorInfoData& InProcessorInfoData);
	void BroadcastOnDashboardUpdated(const FDashboardData& InData);
	void BroadcastOnLogEntryAdded(const FLogEntry& InEntry);
	void BroadcastOnProcessorProgressUpdated(float InProgress);
	void BroadcastOnScenarioComparisonReady(const FScenarioComparisonResult& InResult);
	void BroadcastOnScenarioRemainingUpdated(float InRemainingSeconds);
	void BroadcastOnScenarioDeactived();

	void BroadcastOnUITargetChanged(AActor* InTarget);
	void BroadcastOnProcessingTimeChangeRequested(UClass* InProcessorClass, float InProcessingTime);
	void BroadcastOnShortageScenarioStarted(const float InShortageDuration);
	void BroadcastOnProcessingTimeChangeEnded();
	void BroadcastOnScenarioResultExported();

	const AActor* GetCurrentUITarget();

	FORCEINLINE FStorageInfoUpdated& GetOnStorageInfoUpdatedDel() { return OnStorageInfoUpdated; };
	FORCEINLINE FProcessorInfoUpdated& GetOnProcessorInfoUpdatedDel() { return OnProcessorInfoUpdated; };
	FORCEINLINE FDashboardUpdated& GetOnDashboardUpdatedDel() { return OnDashboardUpdated; }
	FORCEINLINE FLogEntryAdded& GetOnLogEntryAddedDel() { return OnLogEntryAdded; }
	FORCEINLINE FProcessorProgressUpdated& GetOnProcessorProgressUpdatedDel() { return OnProcessorProgressUpdated; }
	FORCEINLINE FScenarioComparisonReady& GetOnScenarioComparisonReady() { return OnScenarioComparisonReady; }
	FORCEINLINE FScenarioRemainingUpdated& GetOnScenarioRemainingUpdated() { return OnScenarioRemainingUpdated; }
	FORCEINLINE FProductionStatExported& GetOnProductionStatExportedDel() { return OnProductionStatExported; }
	FORCEINLINE FSimulationStateUIChanged& GetOnSimulationStateUIChangedDel() { return OnSimulationStateUIChanged; }
	FORCEINLINE FScenarioDeactived& GetOnScenarioDeactived() { return OnScenarioDeactived; }

	FORCEINLINE FUITargetChanged& GetOnUITargetChangedDel() { return OnUITargetChanged; };
	FORCEINLINE FProcessingTimeChangeRequested& GetOnProcessingTimeChangeRequestedDel() { return OnProcessingTimeChangeRequested; }
	FORCEINLINE FShortageScenarioStarted& GetOnShortageScenarioStart() { return OnShortageScenarioStarted; }
	FORCEINLINE FProcessingTimeChangeEnded& GetOnProcessingTimeChangeEnded() { return OnProcessingTimeChangeEnded; };
	FORCEINLINE FScenarioResultExported& GetOnScenarioResultExported() { return OnScenarioResultExported; };
	
	FORCEINLINE FUITargetGotten& GetOnUITargetGotten() { return OnUITargetGotten; };

private:
	FUITargetChanged OnUITargetChanged;
	FProcessingTimeChangeRequested OnProcessingTimeChangeRequested;
	FShortageScenarioStarted OnShortageScenarioStarted;
	FProcessingTimeChangeEnded OnProcessingTimeChangeEnded;
	FScenarioResultExported OnScenarioResultExported;

	FUITargetGotten OnUITargetGotten;
};
