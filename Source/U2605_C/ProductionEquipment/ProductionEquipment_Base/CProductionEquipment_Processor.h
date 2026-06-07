#pragma once

#include "CoreMinimal.h"
#include "ProductionEquipment/CProductionEquipment_Base.h"
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

private:
	UFUNCTION()
	void OnProcessingTimeChangeRequested(UClass* InProcessorClass, float InProcessingTime);

public:
	ACProductionEquipment_Processor();

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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
	float PendingProcessingTime = -1.0f;
};
