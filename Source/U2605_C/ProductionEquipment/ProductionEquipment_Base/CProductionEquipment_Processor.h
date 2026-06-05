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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processor")
	TMap<EProductType, int32> RequiredProducts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processor")
	EProductType ProducedProducts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Processor|Runtime")
	EEquipmentState State = EEquipmentState::Idle;

public:
	void ReceiveProduct(const FProductData& InProductData) override;

private:
	void OnProcessingComplete();
	bool CanStartProcessing() const;

protected:
	void BroadcastInfo() override;

private:
	FTimerHandle ProcessingHandle;
	TMap<EProductType, TArray<FProductData>> ArrivedProducts;
	float ProcessingEndTime = 0.0f;
};
