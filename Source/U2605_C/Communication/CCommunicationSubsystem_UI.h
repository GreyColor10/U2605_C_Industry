#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StructData/CStructDatas.h"
#include "CCommunicationSubsystem_UI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTotalProductCountUpdated, int, InTotalProductNum);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStorageInfoUpdated, const FStorageInfoData&, InStorageInfoData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProcessorInfoUpdated, const FProcessorInfoData&, InProcessorInfoData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FProcessingTimeChangeRequested, UClass*, InProcessorClass, float, InProcessingTime);
DECLARE_DELEGATE_OneParam(FUITargetChanged, AActor*);
DECLARE_DELEGATE_RetVal(const AActor*, FUITargetGotten)

UCLASS()
class U2605_C_API UCCommunicationSubsystem_UI : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	FTotalProductCountUpdated OnTotalProductCountUpdated;

	UPROPERTY()
	FStorageInfoUpdated OnStorageInfoUpdated;

	UPROPERTY()
	FProcessorInfoUpdated OnProcessorInfoUpdated;

	UPROPERTY()
	FProcessingTimeChangeRequested OnProcessingTimeChangeRequested;

public:
	void BroadcastOnTotalProductCountUpdated(int InTotalProductNum);
	void BroadcastOnStorageInfoUpdated(const FStorageInfoData& InStorageInfoData);
	void BroadcastOnProcessorInfoUpdated(const FProcessorInfoData& InProcessorInfoData);
	void BroadcastOnProcessingTimeChangeRequested(UClass* InProcessorClass, float InProcessingTime);

	void BroadcastOnUITargetChanged(AActor* InTarget);
	const AActor* GetCurrentUITarget();

	FORCEINLINE FTotalProductCountUpdated& GetOnTotalProductCountUpdatedDel() { return OnTotalProductCountUpdated; };
	FORCEINLINE FStorageInfoUpdated& GetOnStorageInfoUpdatedDel() { return OnStorageInfoUpdated; };
	FORCEINLINE FProcessorInfoUpdated& GetOnProcessorInfoUpdatedDel() { return OnProcessorInfoUpdated; };
	FORCEINLINE FProcessingTimeChangeRequested& GetOnProcessingTimeChangeRequestedDel() { return OnProcessingTimeChangeRequested; }

	FORCEINLINE FUITargetChanged& GetOnUITargetChangedDel() { return OnUITargetChanged; };
	FORCEINLINE FUITargetGotten& GetOnUITargetGotten() { return OnUITargetGotten; };

private:
	FUITargetChanged OnUITargetChanged;
	FUITargetGotten OnUITargetGotten;
};
