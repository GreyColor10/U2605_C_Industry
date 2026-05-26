#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "StructData/CStructDatas.h" 
#include "CProductionStatSubsystem.generated.h"

UCLASS()
class U2605_C_API UCProductionStatSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	void OnProductDelivered(AActor* InDeliveredEquip, const FProductData& InProductData);;

private:
	int TotalManduNum = 0;
};
