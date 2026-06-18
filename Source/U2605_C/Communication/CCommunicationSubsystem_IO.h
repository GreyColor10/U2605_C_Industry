#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StructData/CStructDatas.h" 
#include "CCommunicationSubsystem_IO.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FProductStarted, AActor*,  const FProductData&);

UCLASS()
class U2605_C_API UCCommunicationSubsystem_IO : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	FProductStarted OnProductStarted;

public:
	void BroadcastOnProductStarted(AActor* InStartedEquip, const FProductData& InProductData);
	bool DeliverProductTo(AActor* InTargetActor, const FProductData& InProductData);

	FORCEINLINE FProductStarted& GetProductStartedDel() { return OnProductStarted; };

};
