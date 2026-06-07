#pragma once

#include "CoreMinimal.h"
#include "ProductionEquipment/CProductionEquipment_Base.h"
#include "CProductionEquipment_Storage.generated.h"

UCLASS(Blueprintable)
class U2605_C_API ACProductionEquipment_Storage : public ACProductionEquipment_Base
{
	GENERATED_BODY()

protected:
	// 시작 시 보관되어 있을 초기 상품
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	TArray<FProductData> InitialProducts;

public:
	UFUNCTION(BlueprintCallable, Category = "Storage")
	bool ShipProduct();

public:
	ACProductionEquipment_Storage();

protected:
	virtual void BeginPlay() override;

public:
	void ReceiveProduct(const FProductData& InProductData) override;

private:
	void BroadcastInfo() override;
};
