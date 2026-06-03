#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StructData/CStructDatas.h"
#include "CCommunicationSubsystem_UI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTotalProductCountUpdated, int, InTotalProductNum);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStorageInfoUpdated, const FStorageInfoData&, InStorageInfoData);

UCLASS()
class U2605_C_API UCCommunicationSubsystem_UI : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	FTotalProductCountUpdated OnTotalProductCountUpdated;

	UPROPERTY()
	FStorageInfoUpdated OnStorageInfoUpdated;

public:
	void BroadcastOnTotalProductCountUpdated(int InTotalProductNum);
	void BroadcastOnStorageInfoUpdated(const FStorageInfoData& InStorageInfoData);

	FORCEINLINE FTotalProductCountUpdated& GetOnTotalProductCountUpdatedDel() { return OnTotalProductCountUpdated; };
	FORCEINLINE FStorageInfoUpdated& GetOnStorageInfoUpdatedDel() { return OnStorageInfoUpdated; }
};
