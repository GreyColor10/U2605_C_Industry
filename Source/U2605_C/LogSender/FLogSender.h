#pragma once

#include "CoreMinimal.h"
#include "StructData/CStructDatas.h"

class U2605_C_API FLogSender
{
public:
	void SendLogMessage(UWorld* InWorld, ELogEventType InEventType, FString InLogMessage);
};
