#pragma once

#include "CoreMinimal.h"
#include "ProductionEquipment/CProductionEquipment_Base.h"
#include "CProductionEquipment_Storage.generated.h"

USTRUCT(BlueprintType)
struct FInitialProductSetting
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EProductType ProductType = EProductType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Count = 0;
};


UCLASS(Blueprintable)
class U2605_C_API ACProductionEquipment_Storage : public ACProductionEquipment_Base
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Storage")
	FInitialProductSetting InitialProducts;

	UPROPERTY(EditAnywhere, Category = "Storage|AutoShip")
	float AutoShipInterval = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Storage")
	int32 MaxCapacity = 0;

public:
	ACProductionEquipment_Storage();

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	bool ShipProduct();
	bool ReceiveProduct(const FProductData& InProductData) override;

public:
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
	TArray<FProductData> StoredProducts;
};
