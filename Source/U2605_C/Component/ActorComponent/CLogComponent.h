#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StructData/CStructDatas.h"
#include "CLogComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class U2605_C_API UCLogComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCLogComponent();

	void SendLogMessage(ELogEventType InEventType, FString InLogMessage);
};
