#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StructData/CStructDatas.h"
#include "CCommunicationSubsystem_UI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTotalProductCountUpdated, int, InTotalProductNum);

UCLASS()
class U2605_C_API UCCommunicationSubsystem_UI : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	FTotalProductCountUpdated OnTotalProductCountUpdated;

public:
	void BroadcastOnTotalProductCountUpdated(int InTotalProductNum);
	FORCEINLINE FTotalProductCountUpdated& GetOnTotalProductCountUpdatedDel() { return OnTotalProductCountUpdated; };
};
