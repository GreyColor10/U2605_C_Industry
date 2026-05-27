#pragma once

#include "CoreMinimal.h"
#include "ProductionEquipment/CProductionEquipment_Base.h"
#include "Types/CFacilityType.h"
#include "CProductionEquipment_Processor.generated.h"

UCLASS(Blueprintable)
class U2605_C_API ACProductionEquipment_Processor : public ACProductionEquipment_Base
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processor")
	float ProcessingTime = 1.0f;

	// ItemID별 필요 수량 (BP에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processor")
	TMap<int32, int32> RequiredProducts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Processor|Runtime")
	EEquipmentState State = EEquipmentState::Idle;

public:
	void ReceiveProduct(const FProductData& InProductData) override;

private:
	void OnProcessingComplete();
	bool CanStartProcessing() const;

private:
	FTimerHandle ProcessingHandle;

	// 현재 도착한 재료 보관 (ItemID별)
	TMap<int32, TArray<FProductData>> ArrivedProducts;
};
