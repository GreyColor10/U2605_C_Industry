#pragma once

#include "CoreMinimal.h"
#include "ProductionEquipment/CProductionEquipment_Base.h"
#include "CProductionEquipment_Storage.generated.h"

UCLASS(Blueprintable)
class U2605_C_API ACProductionEquipment_Storage : public ACProductionEquipment_Base
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	TArray<FProductData> InitialProducts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage|AutoShip")
	float AutoShipInterval = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	int32 MaxCapacity = 0;

public:
	UFUNCTION(BlueprintCallable, Category = "Storage")
	bool ShipProduct();

public:
	ACProductionEquipment_Storage();

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void ReceiveProduct(const FProductData& InProductData) override;

	void StartAutoShip();
	void PauseAutoShip();
	void ResumeAutoShip();
	void StopAutoShip();

private:
	bool IsFull() const;
	void BroadcastInfo() override;

private:
	void OnAutoShipTick();
	void OnSimulationStateChanged(bool InIsRunning);

private:
	FTimerHandle AutoShipHandle;
};
