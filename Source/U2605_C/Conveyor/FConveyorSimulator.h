#pragma once

#include "CoreMinimal.h"
#include "StructData/CStructDatas.h" 

class UCConveyorGraph;
class USplineComponent;

struct FProductOnConvoyor
{
	FProductData ProductData;
	TWeakObjectPtr<AActor> CurrentConveyor;
	TWeakObjectPtr<AActor> EntryConveyor;
	int32 SlotIndex = INDEX_NONE;
};

struct FProductArrival
{
	FProductData ProductData;
	FVector ArrivalLocation = FVector::ZeroVector;
	int32 SimulatorIndex = -1;
};

class FConveyorSimulator
{
public:
	FConveyorSimulator();

public:
	void AddProductAtEntry(AActor* InEntryConveyor, const FProductData& InProductData);
	void Step(UCConveyorGraph* InGraph, TArray<FProductArrival>& OutArrived);
	void SnapshotPositions(UCConveyorGraph* InGraph, TArray<FVector>& OutPositions, TArray<int32>& OutMeshIndices);
	void RemoveProducts(const TArray<int32>& InIndices, UCConveyorGraph* InGraph);
	bool IsEntryBlocked(AActor* InEntryConveyor) const;

	FORCEINLINE int32 Num() const { return ProductsOnConveyer.Num(); }
	FORCEINLINE bool IsEmpty() const { return ProductsOnConveyer.IsEmpty(); }

private:
	void MoveBlockedProduct(const TArray<int32>& InIndices, UCConveyorGraph* InGraph);

private:
	TArray<FProductOnConvoyor> ProductsOnConveyer;
	static constexpr int32 MaxSlots = 50;
	TArray<bool> SlotInUse;
};
