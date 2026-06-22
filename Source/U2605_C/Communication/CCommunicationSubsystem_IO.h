#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StructData/CStructDatas.h" 
#include "CCommunicationSubsystem_IO.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FProductStarted, AActor*,  const FProductData&);
DECLARE_DELEGATE_RetVal_OneParam(const bool, FShipBlocked, const FVector&);

UCLASS()
class U2605_C_API UCCommunicationSubsystem_IO : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void BroadcastOnProductStarted(AActor* InStartedEquip, const FProductData& InProductData);
	const bool IsShipBlocked(const FVector& InSourceLocation);

	bool DeliverProductTo(AActor* InTargetActor, const FProductData& InProductData);

	FORCEINLINE FProductStarted& GetOnProductStartedDel() { return OnProductStarted; };
	FORCEINLINE FShipBlocked& GetOnShipBlockedDel() { return OnShipBlocked; };

private:
	FProductStarted OnProductStarted;

	FShipBlocked OnShipBlocked;
};
