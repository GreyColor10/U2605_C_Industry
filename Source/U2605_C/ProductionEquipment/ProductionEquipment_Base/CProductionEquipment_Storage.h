#pragma once

#include "CoreMinimal.h"
#include "ProductionEquipment/CProductionEquipment_Base.h"
#include "StructData/CStructDatas.h" 
#include "CProductionEquipment_Storage.generated.h"

UCLASS()
class U2605_C_API ACProductionEquipment_Storage : public ACProductionEquipment_Base
{
	GENERATED_BODY()

protected:
	// 시작 시 보관되어 있을 초기 상품 (출고 테스트용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	TArray<FProductData> InitialProducts;

	// 최대 보관 수 (0 이하 = 무제한)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	int32 MaxCapacity = 0;

	// 런타임 보관 목록
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Storage|Runtime")
	TArray<FProductData> StoredProducts;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// 출고 — 외부에서 호출 (디버그 OnClick, 추후 UI 가동 버튼)
	UFUNCTION(BlueprintCallable, Category = "Storage")
	bool ShipProduct();

	UFUNCTION(BlueprintPure, Category = "Storage")
	bool IsFull() const;

	UFUNCTION(BlueprintPure, Category = "Storage")
	FORCEINLINE int32 GetStoredCount() const { return StoredProducts.Num(); }

	UFUNCTION(BlueprintPure, Category = "Storage")
	FORCEINLINE bool IsEmpty() const { return StoredProducts.IsEmpty(); }

private:
	void OnProductDelivered(AActor* InTargetStorage, const FProductData& InProductData);
};
