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
	newProduct.EntryConveyor = InEntryConveyor;

	ProductsOnConveyer.Add(newProduct);
}

void FConveyorSimulator::Step(UCConveyorGraph* InGraph, TArray<FProductArrival>& OutArrived)
{
	CheckNotValid(InGraph);

	TMap<AActor*, TArray<int32>> lineGroups;
	for (int32 i = 0; i < ProductsOnConveyer.Num(); i++)
	{
		AActor* entry = ProductsOnConveyer[i].EntryConveyor.Get();
		if (!entry) continue;
		lineGroups.FindOrAdd(entry).Add(i);
	}

	for (auto& groupPair : lineGroups)
	{
		TArray<int32>& indices = groupPair.Value;

		// 라인 내 GlobalDistance 내림차순 정렬 (Sink에 가까운 순)
		indices.Sort([&](int32 A, int32 B)
			{
				const FProductOnConvoyor& pa = ProductsOnConveyer[A];
				const FProductOnConvoyor& pb = ProductsOnConveyer[B];
				FConveyorNodeInfo* infoA = InGraph->FindNode(pa.CurrentConveyor.Get());
				FConveyorNodeInfo* infoB = InGraph->FindNode(pb.CurrentConveyor.Get());
				float globalA = infoA ? infoA->BaseDistance + pa.ProductData.CurrentDistance : 0.0f;
				float globalB = infoB ? infoB->BaseDistance + pb.ProductData.CurrentDistance : 0.0f;
				return globalA > globalB;
			});

		float prevGlobalDistance = FLT_MAX;

		for (int32 idx : indices)
		{
			FProductOnConvoyor& item = ProductsOnConveyer[idx];
			FProductData& data = item.ProductData;

			if (!item.CurrentConveyor.IsValid()) continue;
			FConveyorNodeInfo* currInfo = InGraph->FindNode(item.CurrentConveyor.Get());
			if (!currInfo) continue;

			float currentGlobal = currInfo->BaseDistance + data.CurrentDistance;

			// 정체 체크
			if (FMath::IsNearlyEqual(prevGlobalDistance - currentGlobal, GridConstants::HalfGridSize) 
				|| prevGlobalDistance - currentGlobal < GridConstants::HalfGridSize)
			{
				prevGlobalDistance = currentGlobal;
				data.bBlocked = true;
				continue;
			}

			// 전진
			data.bBlocked = false;
			if(!data.bArrived) data.CurrentDistance += GridConstants::HalfGridSize;
			if (FMath::IsNearlyZero(data.CurrentDistance)) data.CurrentDistance = 0.0f;
			float remainingDistance = currInfo->NodeDistance - data.CurrentDistance;

			if (FMath::IsNearlyZero(remainingDistance) || remainingDistance < 0.0f)
			{
				if (currInfo->NextConveyor.IsValid())
				{
					FConveyorNodeInfo* nextInfo = InGraph->FindNode(currInfo->NextConveyor.Get());
					if (!nextInfo) continue;

					if (FMath::IsNearlyZero(remainingDistance)) data.CurrentDistance = 0.0f;
					else data.CurrentDistance = FMath::Abs(remainingDistance);

					item.CurrentConveyor = currInfo->NextConveyor;
					prevGlobalDistance = nextInfo->BaseDistance + data.CurrentDistance;
				}
				else
				{
					if (!data.bArrived)
					{
						data.bArrived = true;
						prevGlobalDistance = currInfo->BaseDistance + data.CurrentDistance;
					}

					else
					{
						FProductArrival arrival;
						arrival.ProductData = data;
						arrival.ArrivalLocation = currInfo->SinkPosition;
						arrival.SimulatorIndex = idx;
						OutArrived.Add(arrival);
						prevGlobalDistance = currentGlobal;
					}
				}
			}

			else
			{
				prevGlobalDistance = currInfo->BaseDistance + data.CurrentDistance;
			}
		}
	}
}

void FConveyorSimulator::SnapshotPositions(UCConveyorGraph* InGraph, TArray<FVector>& OutPositions, TArray<int32>& OutMeshIndices)
{
	CheckNotValid(InGraph);

	OutPositions.Reserve(ProductsOnConveyer.Num());
	OutMeshIndices.Reserve(ProductsOnConveyer.Num());

	for (FProductOnConvoyor& item : ProductsOnConveyer)
	{
		if (!item.CurrentConveyor.IsValid()) continue;

		FConveyorNodeInfo* info = InGraph->FindNode(item.CurrentConveyor.Get());
		if (!info) continue;

		AddLocationArray(info->SplineComponent.Get(), item.ProductData, OutPositions, OutMeshIndices);
	}
}

void FConveyorSimulator::RemoveProducts(const TArray<int32>& InIndices, UCConveyorGraph* InGraph)
{
	MoveBlockedProduct(InIndices, InGraph);
	TArray<int32> sorted = InIndices;
	sorted.Sort([](int32 A, int32 B) { return A > B; });
	for (int32 idx : sorted)
		ProductsOnConveyer.RemoveAt(idx);
}

void FConveyorSimulator::AddLocationArray(USplineComponent* InSplineComp, FProductData& InProductData, TArray<FVector>& InLocations, TArray<int32>& InMeshIndices)
{
	CheckNotValid(InSplineComp);

	FVector currLocation = InSplineComp->GetLocationAtDistanceAlongSpline(InProductData.CurrentDistance, ESplineCoordinateSpace::World);
	currLocation.Z += 150.f;
	InLocations.Add(currLocation);
	InMeshIndices.Add(InProductData.ProcessStage);
}

void FConveyorSimulator::MoveBlockedProduct(const TArray<int32>& InIndices, UCConveyorGraph* InGraph )
{
	TSet<AActor*> acceptedLines;
	for (int32 idx : InIndices)
	{
		AActor* entry = ProductsOnConveyer[idx].EntryConveyor.Get();
		if (!entry) continue;
		
		acceptedLines.Add(entry);
	}

	CheckNotValid(InGraph);

	for (FProductOnConvoyor& productOnCon : ProductsOnConveyer)
	{
		if(!acceptedLines.Contains(productOnCon.EntryConveyor.Get())) continue;

		FProductData& data = productOnCon.ProductData;
		if (!data.bBlocked) continue;

		if (!productOnCon.CurrentConveyor.IsValid()) continue;
		FConveyorNodeInfo* currInfo = InGraph->FindNode(productOnCon.CurrentConveyor.Get());

		data.CurrentDistance += GridConstants::HalfGridSize;
		float remainingDistance = currInfo->NodeDistance - data.CurrentDistance;

		if (FMath::IsNearlyZero(remainingDistance) || remainingDistance < 0.0f)
		{
			if (currInfo->NextConveyor.IsValid())
			{
				FConveyorNodeInfo* nextInfo = InGraph->FindNode(currInfo->NextConveyor.Get());
				if (!nextInfo) continue;

				if (FMath::IsNearlyZero(remainingDistance)) data.CurrentDistance = 0.0f;
				else data.CurrentDistance = FMath::Abs(remainingDistance);

				productOnCon.CurrentConveyor = currInfo->NextConveyor;
			}

			else data.bArrived = true;
		}
	}
}
