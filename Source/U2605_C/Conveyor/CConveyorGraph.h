#pragma once

#include "CoreMinimal.h"
#include "CConveyorGraph.generated.h"

USTRUCT(BlueprintType)
struct FConveyorNodeInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AActor> ConveyorActor;

	UPROPERTY()
	TWeakObjectPtr<class USplineComponent> SplineComponent;

	//컨베이어의 이동 지점의 시작과 끝을 담는 배열
	UPROPERTY()
	FIntVector IntPosion[2];

	TWeakObjectPtr<AActor> NextConveyor;
	bool bIsEntryNode = true;
	float NodeDistance = 0;
};

UCLASS()
class U2605_C_API UCConveyorGraph : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TMap<AActor*, FConveyorNodeInfo> ConveyorMap;

	TArray<TWeakObjectPtr<AActor>> Sinks;

public:
	void RegisterNode(AActor* InActor, class USplineComponent* InSpline, const TArray<FIntVector>& InGridPoints);
	void Build();

	FConveyorNodeInfo* FindNode(AActor* InActor);
	FConveyorNodeInfo* FindEntryNode();

	void RegisterSink(AActor* InSink);
	void UnregisterSink(AActor* InSink);

	void FindEntryNodesConnectedTo(const FVector& InLocation, TArray<FConveyorNodeInfo*>& OutNodes);
	void FindSinksConnectedTo(const FVector& InLocation, TArray<AActor*>& OutSinks);

	FORCEINLINE const TMap<AActor*, FConveyorNodeInfo>& GetMap() const { return ConveyorMap; }
};
