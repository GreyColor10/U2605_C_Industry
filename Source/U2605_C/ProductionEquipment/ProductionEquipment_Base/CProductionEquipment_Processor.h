#pragma once

#include "CoreMinimal.h"
#include "ProductionEquipment/CProductionEquipment_Base.h"
#include "CProductionEquipment_Processor.generated.h"

UCLASS(Blueprintable)
class U2605_C_API ACProductionEquipment_Processor : public ACProductionEquipment_Base
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capsule")
	TObjectPtr<class UCProcessingComponent> ProcessingComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processing")
	float ProcessingTime = 1.0f;

private:
	UFUNCTION()
	void OnProcessingTimeChangeRequested(UClass* InProcessorClass, float InProcessingTime);

	UFUNCTION()
	void OnSimulationStateChanged(bool InIsRunning);

public:
	ACProductionEquipment_Processor();

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void ReceiveProduct(const FProductData& InProductData) override;

private:
	void OnProcessingComplete();
	void OnProgressTick();

protected:
	void BroadcastInfo() override;

private:
	FTimerHandle ProcessingHandle;
	FTimerHandle ProgressHandle;

	TMap<EProductType, TArray<FProductData>> ArrivedProducts;

	float PendingProcessingTime = -1.0f;
	float ProcessingStartTime = 0.0f;
	float PausedProcessingTime = 0.0f;
};
