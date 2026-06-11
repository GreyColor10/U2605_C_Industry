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

DECLARE_DELEGATE_OneParam(FUITargetChanged, AActor*);
DECLARE_DELEGATE_RetVal(const AActor*, FUITargetGotten)
DECLARE_MULTICAST_DELEGATE_TwoParams(FProcessingTimeChangeRequested, UClass*, float);
DECLARE_MULTICAST_DELEGATE(FProcessingTimeChangeEnded);
DECLARE_MULTICAST_DELEGATE_OneParam(FSimulationStateChanged, bool);

UCLASS()
class U2605_C_API UCCommunicationSubsystem_UI : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	FStoredFinalProductUpdated OnStoredFinalProductUpdated;

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

public:
	void BroadcastOnStoredFinalProductUpdated(int InStoredFinalProductNum);
	void BroadcastOnStorageInfoUpdated(const FStorageInfoData& InStorageInfoData);
	void BroadcastOnProcessorInfoUpdated(const FProcessorInfoData& InProcessorInfoData);
	void BroadcastOnDashboardUpdated(const FDashboardData& InData);
	void BroadcastOnLogEntryAdded(const FLogEntry& InEntry);
	void BroadcastOnProcessorProgressUpdated(float InProgress);

	void BroadcastOnUITargetChanged(AActor* InTarget);
	const AActor* GetCurrentUITarget();
	void BroadcastOnProcessingTimeChangeRequested(UClass* InProcessorClass, float InProcessingTime);
	void BroadcastOnProcessingTimeChangeEnded();
	void BroadcastOnSimulationStateChanged(bool InIsRunning);

	FORCEINLINE FStoredFinalProductUpdated& GetOnStoredFinalProductUpdatedDel() { return OnStoredFinalProductUpdated; };
	FORCEINLINE FStorageInfoUpdated& GetOnStorageInfoUpdatedDel() { return OnStorageInfoUpdated; };
	FORCEINLINE FProcessorInfoUpdated& GetOnProcessorInfoUpdatedDel() { return OnProcessorInfoUpdated; };
	FORCEINLINE FDashboardUpdated& GetOnDashboardUpdatedDel() { return OnDashboardUpdated; }
	FORCEINLINE FLogEntryAdded& GetOnLogEntryAddedDel() { return OnLogEntryAdded; }
	FORCEINLINE FProcessorProgressUpdated& GetOnProcessorProgressUpdatedDel() { return OnProcessorProgressUpdated; }
	
	FORCEINLINE FUITargetChanged& GetOnUITargetChangedDel() { return OnUITargetChanged; };
	FORCEINLINE FUITargetGotten& GetOnUITargetGotten() { return OnUITargetGotten; };
	FORCEINLINE FProcessingTimeChangeRequested& GetOnProcessingTimeChangeRequestedDel() { return OnProcessingTimeChangeRequested; }
	FORCEINLINE FProcessingTimeChangeEnded& GetOnProcessingTimeChangeEnded() { return OnProcessingTimeChangeEnded; };
	FORCEINLINE FSimulationStateChanged& GetOnSimulationStateChangedDel() { return OnSimulationStateChanged; }

private:
	FUITargetChanged OnUITargetChanged;
	FUITargetGotten OnUITargetGotten;
	FProcessingTimeChangeRequested OnProcessingTimeChangeRequested;
	FProcessingTimeChangeEnded OnProcessingTimeChangeEnded;
	FSimulationStateChanged OnSimulationStateChanged;
};
