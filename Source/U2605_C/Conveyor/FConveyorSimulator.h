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
	bool AddDefaultProduct(UCConveyorGraph* InGraph);

	// 신규: 특정 진입 컨베이어에 상품 추가
	void AddProductAtEntry(AActor* InEntryConveyor, const FProductData& InProductData);

	// 시그니처 변경: OutArrived가 FProductArrival 배열로
	void Step(UCConveyorGraph* InGraph, TArray<FVector>& OutPositions, TArray<FProductArrival>& OutArrived);

	FORCEINLINE int32 Num() const { return ProductsOnConveyer.Num(); }
	FORCEINLINE bool IsEmpty() const { return ProductsOnConveyer.IsEmpty(); }

private:
	void AddLocationArray(USplineComponent* InSplineComp, FProductData& InProductData, TArray<FVector>& InLocations);

private:
	TArray<FProductOnConvoyor> ProductsOnConveyer;
};
