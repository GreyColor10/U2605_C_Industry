#include "Conveyor/CConveyorSubsystem.h"
#include "Global.h"

#include "Conveyor/CConveyorGraph.h"
#include "Communication/CCommunicationSubsystem_IO.h"
#include "Communication/CCommunicationSubsystem_UI.h"

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

	ioSubsystem->GetOnProductStartedDel().AddUObject(this, &UCConveyorSubsystem::OnProductStarted);
	ioSubsystem->GetOnShipBlockedDel().BindUObject(this, &UCConveyorSubsystem::IsShipBlockedFrom);

	UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(uiSubsystem);

	uiSubsystem->GetOnSimulationStateChangedDel().AddUObject(this, &UCConveyorSubsystem::OnSimulationStateChanged);
}

void UCConveyorSubsystem::Deinitialize()
{
	UWorld* world = GetWorld();
	if (IsValid(world))
	{
		UGameInstance* game = world->GetGameInstance();
		if (IsValid(game))
		{
			UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>();
			if (IsValid(ioSubsystem))
			{
				ioSubsystem->GetOnProductStartedDel().RemoveAll(this);
				ioSubsystem->GetOnShipBlockedDel().Unbind();
			}
			
			UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
			if (IsValid(uiSubsystem))
				uiSubsystem->GetOnSimulationStateChangedDel().RemoveAll(this);
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

void UCConveyorSubsystem::Pause()
{
	CheckTrue(bIsPaused);
	bIsPaused = true;

	UWorld* world = GetWorld();
	CheckNotValid(world);

	world->GetTimerManager().PauseTimer(ConveyorHandle);

	if (OnNiagaraCompActive.IsBound())
		OnNiagaraCompActive.Broadcast(false);
}

void UCConveyorSubsystem::Resume()
{
	CheckFalse(bIsPaused);
	bIsPaused = false;

	UWorld* world = GetWorld();
	CheckNotValid(world);

	world->GetTimerManager().UnPauseTimer(ConveyorHandle);

	CheckNull(Simulator);
	if (!Simulator->IsEmpty())
	{
		TArray<FVector> locationsArray;
		TArray<int32> meshIndicesArray;
		TArray<int32> idsArray;
		Simulator->SnapshotPositions(Graph, locationsArray, meshIndicesArray);

		if (OnNiagaraCompActive.IsBound())
			OnNiagaraCompActive.Broadcast(true);

		if (OnNiagaraCompSetParticlePosition.IsBound())
			OnNiagaraCompSetParticlePosition.Broadcast(TEXT("DataPositions"), locationsArray);

		if (OnNiagaraCompSetMeshIndices.IsBound())
			OnNiagaraCompSetMeshIndices.Broadcast(TEXT("MeshIndices"), meshIndicesArray);
	}
}

void UCConveyorSubsystem::StartSimulationIfNeeded()
{
	CheckNull(Simulator);

	UWorld* world = GetWorld();
	CheckNotValid(world);

	FTimerManager& manager = world->GetTimerManager();
	CheckTrue(manager.TimerExists(ConveyorHandle));

	if (OnNiagaraCompActive.IsBound())
		OnNiagaraCompActive.Broadcast(true);

	FTimerDelegate del;
	del.BindUObject(this, &UCConveyorSubsystem::UpdateProductItemsFlow);
	manager.SetTimer(ConveyorHandle, del, 0.5f, true, 0.0f);
}

void UCConveyorSubsystem::UpdateProductItemsFlow()
{
	TArray<FProductArrival> arrivedProducts;

	CheckNull(Simulator);
	Simulator->Step(Graph, arrivedProducts);

	DeliverArrivedProducts(arrivedProducts);

	TArray<FVector> locationsArray;
	TArray<int32> meshIndicesArray;
	TArray<int32> idsArray;
	Simulator->SnapshotPositions(Graph, locationsArray, meshIndicesArray);

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

	TArray<int32> acceptedIndices;

	for (FProductArrival& arrival : InArrived)
	{
		TArray<AActor*> sinks;
		Graph->FindSinksConnectedTo(arrival.ArrivalLocation, sinks);

		for (AActor* sink : sinks)
			if (ioSubsystem->DeliverProductTo(sink, arrival.ProductData))
			{
				acceptedIndices.Add(arrival.SimulatorIndex);
				break;
			}	
	}

	if(!acceptedIndices.IsEmpty())
		Simulator->RemoveProducts(acceptedIndices, Graph);
}

const bool UCConveyorSubsystem::IsShipBlockedFrom(const FVector& InSourceLocation) const
{
	CheckNotValidResult(Graph, false);
	CheckNullResult(Simulator, false);

	TArray<FConveyorNodeInfo*> entryNodes;
	Graph->FindEntryNodesConnectedTo(InSourceLocation, entryNodes);

	for (FConveyorNodeInfo* entryNode : entryNodes)
	{
		if (!entryNode->ConveyorActor.IsValid()) continue;
		if (Simulator->IsEntryBlocked(entryNode->ConveyorActor.Get()))
			return true;
	}

	return false;
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

void UCConveyorSubsystem::OnSimulationStateChanged(bool InIsRunning)
{
	if (InIsRunning) Resume();
	else Pause();
}