#include "ProductionEquipment/ProductionEquipment_Base/CProductionEquipment_Processor.h"
#include "Global.h"

#include "MeshInstancing/CInstancedMeshSubsystem.h"
#include "Communication/CCommunicationSubsystem_IO.h"
#include "Communication/CCommunicationSubsystem_UI.h"

void ACProductionEquipment_Processor::OnProcessingTimeChangeRequested(UClass* InProcessorClass, float InProcessingTime)
{
	if (GetClass() != InProcessorClass) return;

	if (State == EEquipmentState::Processing)
	{
		PendingProcessingTime = InProcessingTime;
		return;
	}

	ProcessingTime = InProcessingTime;
}

ACProductionEquipment_Processor::ACProductionEquipment_Processor()
{
	InfoUIType = EInfoUIType::Processor;
}

void ACProductionEquipment_Processor::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* game = GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	commuSubsystem_UI->GetOnProcessingTimeChangeRequestedDel().AddDynamic(this, &ACProductionEquipment_Processor::OnProcessingTimeChangeRequested);
}

void ACProductionEquipment_Processor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameInstance* game = GetGameInstance();
	if (game)
	{
		UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
		if (commuSubsystem_UI)
			commuSubsystem_UI->GetOnProcessingTimeChangeRequestedDel().RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ACProductionEquipment_Processor::ReceiveProduct(const FProductData& InProductData)
{
	CheckFalse(State == EEquipmentState::Idle);

	CheckFalse(RequiredProducts.Contains(InProductData.ProductType));

	ArrivedProducts.FindOrAdd(InProductData.ProductType).Add(InProductData);

	if (!CanStartProcessing())
	{
		UITargetBroadcastInfo();
		return;
	}
	
	State = EEquipmentState::Processing;
	ProcessingEndTime = GetWorld()->GetTimeSeconds() + ProcessingTime;
	UITargetBroadcastInfo();

	UWorld* world = GetWorld();
	CheckNotValid(world);

	world->GetTimerManager().SetTimer(
		ProcessingHandle,
		this,
		&ACProductionEquipment_Processor::OnProcessingComplete,
		ProcessingTime,
		false
	);

	UCInstancedMeshSubsystem* instancingSubsystem = world->GetSubsystem<UCInstancedMeshSubsystem>();
	CheckNotValid(instancingSubsystem);

	instancingSubsystem->SetCustomData(InstancingMesh, HISMInstanceIndex, 0, 1.0f);
}

void ACProductionEquipment_Processor::OnProcessingComplete()
{
	if (PendingProcessingTime > 0.0f)
	{
		ProcessingTime = PendingProcessingTime;
		PendingProcessingTime = -1.0f;
	}

	State = EEquipmentState::Idle;
	ProcessingEndTime = 0.0f;

	for (auto& pair : RequiredProducts)
	{
		TArray<FProductData>& arrived = ArrivedProducts[pair.Key];
		arrived.RemoveAt(0, pair.Value, EAllowShrinking::No);
	}

	UITargetBroadcastInfo();

	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>();
	CheckNotValid(ioSubsystem);

	FProductData processedProduct;
	processedProduct.CurrentDistance = 0.0f;
	processedProduct.bArrived = false;
	processedProduct.ProductType = ProducedProducts;
	processedProduct.ProcessStage = (int)ProducedProducts - 1;

	ioSubsystem->BroadcastOnProductStarted(this, processedProduct);

	UCInstancedMeshSubsystem* instancingSubsystem = world->GetSubsystem<UCInstancedMeshSubsystem>();
	CheckNotValid(instancingSubsystem);
	instancingSubsystem->SetCustomData(InstancingMesh, HISMInstanceIndex, 0, 0.0f);
}

bool ACProductionEquipment_Processor::CanStartProcessing() const
{
	for (const auto& pair : RequiredProducts)
	{
		const TArray<FProductData>* arrived = ArrivedProducts.Find(pair.Key);
		if (!arrived) return false;
		if (arrived->Num() < pair.Value) return false;
	}
	return true;
}

void ACProductionEquipment_Processor::BroadcastInfo()
{
	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	FProcessorInfoData infoData;
	infoData.State = State;
	infoData.ProcessingTime = ProcessingTime;
	infoData.ProcessingEndTime = ProcessingEndTime;
	infoData.RequiredProducts = RequiredProducts;
	infoData.ProducedProduct = ProducedProducts;

	for (const auto& pair : ArrivedProducts)
		infoData.ArrivedCount.Add(pair.Key, pair.Value.Num());

	commuSubsystem_UI->BroadcastOnProcessorInfoUpdated(infoData);
}
