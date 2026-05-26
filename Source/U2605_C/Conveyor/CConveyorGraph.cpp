#include "Conveyor/CConveyorGraph.h"
#include "Global.h"
#include "Components/SplineComponent.h"

void UCConveyorGraph::RegisterNode(AActor* InActor, USplineComponent* InSpline, const TArray<FIntVector>& InGridPoints)
{
	CheckNotValid(InActor);
	CheckNotValid(InSpline);

	FConveyorNodeInfo newNode;
	newNode.ConveyorActor = InActor;
	newNode.SplineComponent = InSpline;
	newNode.IntPosion[0] = InGridPoints[0];
	newNode.IntPosion[1] = InGridPoints[InGridPoints.Num() - 1];
	newNode.NodeDistance = (InSpline->GetNumberOfSplinePoints() - 1) * 50.0f;

	ConveyorMap.Add(InActor, newNode);
}

void UCConveyorGraph::Build()
{
	TMap<FIntVector, AActor*> gridToActorMap;

	for (auto& pair : ConveyorMap)
	{
		pair.Value.bIsEntryNode = true;
		pair.Value.NextConveyor = nullptr;

		gridToActorMap.Add(pair.Value.IntPosion[0], pair.Key);
	}

	for (auto& pair : ConveyorMap)
	{
		FConveyorNodeInfo& currentNode = pair.Value;
		FIntVector myExitTarget = currentNode.IntPosion[1];

		AActor** targetConveyorPtr = gridToActorMap.Find(myExitTarget);
		if (!targetConveyorPtr) continue;

		AActor* targetConveyor = *targetConveyorPtr;
		if (pair.Key == targetConveyor) continue;

		currentNode.NextConveyor = targetConveyor;
		FConveyorNodeInfo* nextNode = ConveyorMap.Find(targetConveyor);
		if (!nextNode) continue;
		nextNode->bIsEntryNode = false;
	}
}

FConveyorNodeInfo* UCConveyorGraph::FindNode(AActor* InActor)
{
	return ConveyorMap.Find(InActor);
}

FConveyorNodeInfo* UCConveyorGraph::FindEntryNode()
{
	for (auto& pair : ConveyorMap)
	{
		if (!pair.Value.bIsEntryNode) continue;
		if (!pair.Value.ConveyorActor.IsValid()) continue;

		return &pair.Value;
	}

	return nullptr;
}

FConveyorNodeInfo* UCConveyorGraph::FindEntryNodeNearestTo(const FVector& InLocation)
{
	FConveyorNodeInfo* bestNode = nullptr;
	float bestDistSq = TNumericLimits<float>::Max();

	for (auto& pair : ConveyorMap)
	{
		FConveyorNodeInfo& node = pair.Value;
		if (!node.bIsEntryNode) continue;
		if (!node.ConveyorActor.IsValid()) continue;
		if (!node.SplineComponent.IsValid()) continue;

		// Spline 시작 지점의 월드 좌표
		FVector startWorld = node.SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
		float distSq = FVector::DistSquared(InLocation, startWorld);

		if (distSq < bestDistSq)
		{
			bestDistSq = distSq;
			bestNode = &node;
		}
	}

	return bestNode;
}

void UCConveyorGraph::RegisterSink(AActor* InSink)
{
	CheckNotValid(InSink);
	Sinks.AddUnique(InSink);
}

void UCConveyorGraph::UnregisterSink(AActor* InSink)
{
	Sinks.RemoveAll([InSink](const TWeakObjectPtr<AActor>& InWeak)
		{
			return !InWeak.IsValid() || InWeak.Get() == InSink;
		});
}

AActor* UCConveyorGraph::FindNearestSinkTo(const FVector& InLocation)
{
	AActor* bestSink = nullptr;
	float bestDistSq = TNumericLimits<float>::Max();

	for (const TWeakObjectPtr<AActor>& weakSink : Sinks)
	{
		if (!weakSink.IsValid()) continue;
		AActor* sink = weakSink.Get();

		float distSq = FVector::DistSquared(InLocation, sink->GetActorLocation());
		if (distSq < bestDistSq)
		{
			bestDistSq = distSq;
			bestSink = sink;
		}
	}

	return bestSink;
}
