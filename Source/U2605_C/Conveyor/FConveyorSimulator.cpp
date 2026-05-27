#include "Conveyor/FConveyorSimulator.h"
#include "Global.h"
#include "Components/SplineComponent.h"

#include "Conveyor/CConveyorGraph.h"

void FConveyorSimulator::AddProductAtEntry(AActor* InEntryConveyor, const FProductData& InProductData)
{
	if (!IsValid(InEntryConveyor)) return;

	FProductOnConvoyor newProduct;
	newProduct.ProductData = InProductData;
	newProduct.CurrentConveyor = InEntryConveyor;

	ProductsOnConveyer.Add(newProduct);
}

void FConveyorSimulator::Step(UCConveyorGraph* InGraph, TArray<FVector>& OutPositions, TArray<FProductArrival>& OutArrived)
{
	CheckNotValid(InGraph);

	OutPositions.Reserve(ProductsOnConveyer.Num());

	for (int32 i = ProductsOnConveyer.Num() - 1; i >= 0; --i)
	{
		FProductData& data = ProductsOnConveyer[i].ProductData;
		TWeakObjectPtr<AActor> currConveyor = ProductsOnConveyer[i].CurrentConveyor;
		if (!currConveyor.IsValid()) continue;

		FConveyorNodeInfo* currInfo = InGraph->FindNode(currConveyor.Get());
		if (!currInfo) continue;

		data.CurrentDistance += data.MoveSpeed * 1.0f;
		float remainingDistance = currInfo->NodeDistance - data.CurrentDistance;

		//현재 컨베이어의 끝에 도달했을 경우
		if (remainingDistance < 0 || FMath::IsNearlyZero(remainingDistance))
		{
			// 이어진 컨베이어가 있을 때
			if (currInfo->NextConveyor.IsValid())
			{
				FConveyorNodeInfo* nextInfo = InGraph->FindNode(currInfo->NextConveyor.Get());
				if (!nextInfo) continue;

				data.CurrentDistance = 0.0f;
				if (remainingDistance < 0) data.CurrentDistance = abs(remainingDistance);

				TWeakObjectPtr<class USplineComponent> nextSplineComp = nextInfo->SplineComponent;
				if (!nextSplineComp.IsValid()) continue;

				AddLocationArray(nextSplineComp.Get(), data, OutPositions);
				ProductsOnConveyer[i].CurrentConveyor = currInfo->NextConveyor;
				continue;
			}

			// 이어진 컨베이어가 없이 끝에 도달했을 때
			else
			{
				// 처음 마지막 지점에 도착함.
				if (!data.bArrived)
				{
					data.bArrived = true;
					TWeakObjectPtr<USplineComponent> splineComp = currInfo->SplineComponent;
					if (!splineComp.IsValid()) continue;

					AddLocationArray(splineComp.Get(), data, OutPositions);
				}

				// 이미 마지막 지점에 도달한 뒤였다면
				else
				{
					FProductArrival arrival;
					arrival.ProductData = data;
					arrival.ArrivalLocation = currInfo->SinkPosition;
					
					OutArrived.Add(arrival);
					ProductsOnConveyer.RemoveAt(i);
					continue;
				}
			}
		}
		// 현재 컨테이너의 경로가 남아있을 경우
		else
		{
			TWeakObjectPtr<USplineComponent> splineComp = currInfo->SplineComponent;
			if (!splineComp.IsValid()) continue;

			AddLocationArray(splineComp.Get(), data, OutPositions);
		}
	}
}

void FConveyorSimulator::AddLocationArray(USplineComponent* InSplineComp, FProductData& InProductData, TArray<FVector>& InLocations)
{
	FVector currLocation = InSplineComp->GetLocationAtDistanceAlongSpline(InProductData.CurrentDistance, ESplineCoordinateSpace::World);
	currLocation.Z += 10.f;
	InLocations.Add(currLocation);
}
