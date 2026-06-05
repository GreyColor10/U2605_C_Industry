#pragma once

#include "CoreMinimal.h"
#include "StructData/CStructDatas.h" 

class UCConveyorGraph;
class USplineComponent;

struct FProductOnConvoyor
{
	FProductData ProductData;
	TWeakObjectPtr<AActor> CurrentConveyor;
};

struct FProductArrival
{
	FProductData ProductData;
	FVector ArrivalLocation = FVector::ZeroVector;
};

class FConveyorSimulator
{
public:
	void AddProductAtEntry(AActor* InEntryConveyor, const FProductData& InProductData);
	void Step(UCConveyorGraph* InGraph, TArray<FVector>& OutPositions, TArray<int32>& OutMeshIndices, TArray<FProductArrival>& OutArrived);
	void SnapshotPositions(UCConveyorGraph* InGraph, TArray<FVector>& OutPositions, TArray<int32>& OutMeshIndices);

	FORCEINLINE int32 Num() const { return ProductsOnConveyer.Num(); }
	FORCEINLINE bool IsEmpty() const { return ProductsOnConveyer.IsEmpty(); }

private:
	void AddLocationArray(USplineComponent* InSplineComp, FProductData& InProductData, TArray<FVector>& InLocations, TArray<int32>& InMeshIndices);

private:
	TArray<FProductOnConvoyor> ProductsOnConveyer;
};
