#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StructData/CStructDatas.h"
#include "CommunicationSubsystem_UI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShippable, FProductData, InProductData);

UCLASS()
class U2605_C_API UCommunicationSubsystem_UI : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	FShippable OnShippable;

public:
	void BroadcastOnShippable(FProductData& InProductData);
	FORCEINLINE FShippable& GetShippableDel() { return OnShippable; };
};
