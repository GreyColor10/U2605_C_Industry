#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StructData/CStructDatas.h"
#include "CProcessingComponent.generated.h"

UCLASS( ClassGroup=(Custom))
class U2605_C_API UCProcessingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processing")
	TMap<EProductType, int32> RequiredProducts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Processing")
	EProductType ProducedProducts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Processing | Runtime")
	EEquipmentState State = EEquipmentState::Idle;

public:	
	UCProcessingComponent();

public:
	bool StartProcessing(const TMap<EProductType, TArray<FProductData>>& InArrivedProducts, UStaticMesh* InStaticMesh, int32 InMeshIndex);
	FProductData CompleteProcessing(TMap<EProductType, TArray<FProductData>>& InArrivedProducts, UStaticMesh* InStaticMesh, int32 InMeshIndex);
	FProcessorInfoData GetProcessorInfoData() const;

public:
	const FORCEINLINE TMap<EProductType, int32> GetRequiredProducts() const { return RequiredProducts; };

	const FORCEINLINE bool IsIdleState() const { return State == EEquipmentState::Idle; };
	const FORCEINLINE bool IsProcessingState() const { return State == EEquipmentState::Processing; };
private:
	bool CanStartProcessing(const TMap<EProductType, TArray<FProductData>>& InArrivedProducts) const;
};
