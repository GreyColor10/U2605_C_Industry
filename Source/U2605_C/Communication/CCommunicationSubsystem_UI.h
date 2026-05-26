#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StructData/CStructDatas.h"
#include "CCommunicationSubsystem_UI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShippable, int, InTotalManduNum);

UCLASS()
class U2605_C_API UCCommunicationSubsystem_UI : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	FShippable OnShippable;

public:
	void BroadcastOnShippable(int InTotalManduNum);
	FORCEINLINE FShippable& GetShippableDel() { return OnShippable; };
};
