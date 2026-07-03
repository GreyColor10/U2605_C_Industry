#pragma once

#include "CoreMinimal.h"
#include "ProductionEquipment/CProductionEquipment_Base.h"
#include "CProductionEquipment_Processor.generated.h"

UCLASS(Blueprintable)
class U2605_C_API ACProductionEquipment_Processor : public ACProductionEquipment_Base
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Capsule")
	TObjectPtr<class UCProcessingComponent> ProcessingComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Processing")
	float ProcessingTime = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Processing")
	int32 BufferMultiplier = 2;

public:
	ACProductionEquipment_Processor();

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	bool ReceiveProduct(const FProductData& InProductData) override;

private:
	bool TryStartProcessing();

private:
	void OnProcessingComplete();
	void OnProgressTick();

	void OnProcessingTimeChangeRequested(UClass* InProcessorClass, float InProcessingTime);
	void OnProcessingTimeChangeEnded();

protected:
	void BroadcastInfo() override;
	void OnSimulationStateChanged(bool InIsRunning) override;

private:
	void OnShortageScenarioActiveChanged(bool IsActive);

private:
	FTimerHandle ProcessingHandle;
	FTimerHandle ProgressHandle;

	TMap<EProductType, TArray<FProductData>> ArrivedProducts;

	float PendingProcessingTime = -1.0f;
	float ProcessingStartTime = 0.0f;
	float PausedProcessingTime = 0.0f;

	float PrevProcessingTime = 0.0f;
};
