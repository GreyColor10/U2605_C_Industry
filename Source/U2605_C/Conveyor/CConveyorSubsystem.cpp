#include "Conveyor/CConveyorSubsystem.h"
#include "Global.h"

#include "Communication/CommunicationSubsystem_UI.h"
#include "Conveyor/CConveyorGraph.h"
#include "Communication/CommunicationSubsystem_IO.h"

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

	UCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCommunicationSubsystem_IO>();
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
			if (UCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCommunicationSubsystem_IO>())
				ioSubsystem->GetProductStartedDel().RemoveAll(this);
		}
	}
	Super::Deinitialize();
}

void UCConveyorSubsystem::RegisterConveyor(AActor* InActor, USplineComponent* InSpline, TArray<FIntVector>& InGridPoints)
{
	CheckNotValid(Graph);
	Graph->RegisterNode(InActor, InSpline, InGridPoints);
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
	del.BindUObject(this, &UCConveyorSubsystem::UpdatedProductItemsFlow);
	manager.SetTimer(ConveyorHandle, del, 0.5f, true);

	if (OnNiagaraCompActive.IsBound())
		OnNiagaraCompActive.Broadcast(true);
}

void UCConveyorSubsystem::UpdatedProductItemsFlow()
{
	TArray<FVector> locationsArray;
	TArray<FProductArrival> arrivedProducts;

	CheckNull(Simulator);
	Simulator->Step(Graph, locationsArray, arrivedProducts);

	OnBroadCast(arrivedProducts);

	if (OnNiagaraCompSetParameter.IsBound())
		OnNiagaraCompSetParameter.Broadcast(TEXT("DataPositions"), locationsArray);

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

void UCConveyorSubsystem::OnBroadCast(TArray<FProductArrival>& InArrived)
{
	CheckFalse(InArrived.Num() > 0);

	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	UCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCommunicationSubsystem_IO>();
	CheckNotValid(ioSubsystem);

	CheckNotValid(Graph);

	for (FProductArrival& arrival : InArrived)
	{
		// 1) UI 수익 알림 (기존)
		commuSubsystem_UI->BroadcastOnShippable(arrival.ProductData);

		// 2) 도착 지점 근처 Sink 찾기 → 있으면 입고 알림
		AActor* nearestSink = Graph->FindNearestSinkTo(arrival.ArrivalLocation);
		if (IsValid(nearestSink))
		{
			ioSubsystem->BroadcastOnProductDelivered(nearestSink, arrival.ProductData);
		}
	}
}

void UCConveyorSubsystem::OnProductStarted(AActor* InSourceStorage, const FProductData& InProductData)
{
	CheckNotValid(InSourceStorage);
	CheckNotValid(Graph);
	CheckNull(Simulator);

	// Source 위치에서 가장 가까운 진입 컨베이어 찾기
	FConveyorNodeInfo* entryNode = Graph->FindEntryNodeNearestTo(InSourceStorage->GetActorLocation());
	CheckNull(entryNode);
	CheckFalse(entryNode->ConveyorActor.IsValid());

	Simulator->AddProductAtEntry(entryNode->ConveyorActor.Get(), InProductData);

	StartSimulationIfNeeded();
}
