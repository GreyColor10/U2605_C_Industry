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

void UCConveyorGraph::FindEntryNodesConnectedTo(const FVector& InLocation, TArray<FConveyorNodeInfo*>& OutNodes)
{
	for (auto& pair : ConveyorMap)
	{
		FConveyorNodeInfo& node = pair.Value;
		if (!node.bIsEntryNode) continue;
		if (!node.ConveyorActor.IsValid()) continue;
		if (!node.SplineComponent.IsValid()) continue;

		FVector startWorld = node.SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
		float dist = FVector::Dist(InLocation, startWorld);

		if (FMath::IsNearlyEqual(dist, GridConstants::HalfGridSize, GridConstants::ConnectionTolerance))
			OutNodes.Add(&node);
	}
}

void UCConveyorGraph::FindSinksConnectedTo(const FVector& InLocation, TArray<AActor*>& OutSinks)
{
	for (const TWeakObjectPtr<AActor>& weakSink : Sinks)
	{
		if (!weakSink.IsValid()) continue;
		AActor* sink = weakSink.Get();

		float dist = FVector::Dist(InLocation, sink->GetActorLocation());
		if (FMath::IsNearlyEqual(dist, GridConstants::HalfGridSize, GridConstants::ConnectionTolerance))
			OutSinks.Add(sink);
	}
}
