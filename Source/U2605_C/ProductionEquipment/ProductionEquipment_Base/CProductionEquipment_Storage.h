#pragma once

#include "CoreMinimal.h"
#include "ProductionEquipment/CProductionEquipment_Base.h"
#include "CProductionEquipment_Storage.generated.h"

UCLASS(Blueprintable)
class U2605_C_API ACProductionEquipment_Storage : public ACProductionEquipment_Base
{
	GENERATED_BODY()

protected:
	// 시작 시 보관되어 있을 초기 상품 (출고 테스트용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	TArray<FProductData> InitialProducts;

protected:
	virtual void BeginPlay() override;

public:
	void ReceiveProduct(const FProductData& InProductData) override;
};
