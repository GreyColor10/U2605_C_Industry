#include "Conveyor/CConveyorSubsystem.h"
#include "Global.h"

#include "Conveyor/CConveyorGraph.h"
#include "Communication/CCommunicationSubsystem_IO.h"

void UCConveyorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Graph = NewObject<UCConveyorGraph>(this);
	Simulator = MakeUnique<FConveyorSimulator>();

	UWorld* world = GetWorld();
	CheckNull(world);

	world->OnWorldBeginPlay.AddUObject(this, &UCConveyorSubsystem::BuildConveyorNetwork);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>();
	CheckNotValid(ioSubsystem);

	ioSubsystem->GetProductStartedDel().AddUObject(this, &UCConveyorSubsystem::OnProductStarted);
}

void UCConveyorSubsystem::Deinitialize()
{
	UWorld* world = GetWorld();
	if (IsValid(world))
	{
		if (UGameInstance* game = world->GetGameInstance())
		{
			if (UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>())
				ioSubsystem->GetProductStartedDel().RemoveAll(this);
		}
	}
	Super::Deinitialize();
}

void UCConveyorSubsystem::RegisterConveyor(AActor* InActor, class USplineComponent* InSpline, const TArray<FIntVector>& InGridPoints, const FVector InSinkPosition)
{
	CheckNotValid(Graph);
	Graph->RegisterNode(InActor, InSpline, InGridPoints, InSinkPosition);
}

void UCConveyorSubsystem::BuildConveyorNetwork()
{
	CheckNotValid(Graph);
	Graph->Build();
}

void UCConveyorSubsystem::RegisterSink(AActor* InSink)
{
	CheckNotValid(Graph);
	Graph->RegisterSink(InSink);
}

void UCConveyorSubsystem::UnregisterSink(AActor* InSink)
{
	CheckNotValid(Graph);
	Graph->UnregisterSink(InSink);
}

void UCConveyorSubsystem::StartSimulationIfNeeded()
{
	CheckNull(Simulator);

	UWorld* world = GetWorld();
	CheckNotValid(world);

	FTimerManager& manager = world->GetTimerManager();
	CheckTrue(manager.TimerExists(ConveyorHandle));

	//타이머가 시작할 때 나이아가라 컴포넌트도 같이 킴
	FTimerDelegate del;
	del.BindUObject(this, &UCConveyorSubsystem::UpdateProductItemsFlow);
	manager.SetTimer(ConveyorHandle, del, 0.5f, true);

	if (OnNiagaraCompActive.IsBound())
		OnNiagaraCompActive.Broadcast(true);
}

void UCConveyorSubsystem::UpdateProductItemsFlow()
{
	TArray<FVector> locationsArray;
	TArray<int32> meshIndicesArray;
	TArray<FProductArrival> arrivedProducts;

	CheckNull(Simulator);
	Simulator->Step(Graph, locationsArray, meshIndicesArray, arrivedProducts);

	DeliverArrivedProducts(arrivedProducts);

	if (OnNiagaraCompSetParticlePosition.IsBound())
		OnNiagaraCompSetParticlePosition.Broadcast(TEXT("DataPositions"), locationsArray);

	if (OnNiagaraCompSetMeshIndices.IsBound())
		OnNiagaraCompSetMeshIndices.Broadcast(TEXT("MeshIndices"), meshIndicesArray);

	// 모든 상품이 빠지면 시뮬레이션 정지
	if (Simulator->IsEmpty())
	{
		if (OnNiagaraCompActive.IsBound())
			OnNiagaraCompActive.Broadcast(false);

		UWorld* world = GetWorld();
		CheckNotValid(world);

		FTimerManager& manager = world->GetTimerManager();
		manager.ClearTimer(ConveyorHandle);
	}
}

void UCConveyorSubsystem::DeliverArrivedProducts(TArray<FProductArrival>& InArrived)
{
	CheckFalse(InArrived.Num() > 0);

	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>();
	CheckNotValid(ioSubsystem);
	CheckNotValid(Graph);

	for (FProductArrival& arrival : InArrived)
	{
		TArray<AActor*> sinks;
		Graph->FindSinksConnectedTo(arrival.ArrivalLocation, sinks);

		for (AActor* sink : sinks)
			ioSubsystem->DeliverProductTo(sink, arrival.ProductData);
	}
}

void UCConveyorSubsystem::OnProductStarted(AActor* InSourceStorage, const FProductData& InProductData)
{
	CheckNotValid(InSourceStorage);
	CheckNotValid(Graph);
	CheckNull(Simulator);

	TArray<FConveyorNodeInfo*> entryNodes;
	Graph->FindEntryNodesConnectedTo(InSourceStorage->GetActorLocation(), entryNodes);
	CheckFalse(entryNodes.Num() > 0);

	for (FConveyorNodeInfo* entryNode : entryNodes)
	{
		if (!entryNode->ConveyorActor.IsValid()) continue;
		Simulator->AddProductAtEntry(entryNode->ConveyorActor.Get(), InProductData);
	}

	StartSimulationIfNeeded();
}
