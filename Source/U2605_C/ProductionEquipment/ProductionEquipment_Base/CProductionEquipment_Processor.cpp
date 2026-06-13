#include "ProductionEquipment/ProductionEquipment_Base/CProductionEquipment_Processor.h"
#include "Global.h"

#include "Component/ActorComponent/CProcessingComponent.h"
#include "Communication/CCommunicationSubsystem_IO.h"
#include "Communication/CCommunicationSubsystem_UI.h"
#include "ProductionStat/CProductionStatSubsystem.h"
#include "SimulationTime/CSimulationTimeSubsystem.h"

ACProductionEquipment_Processor::ACProductionEquipment_Processor()
{
	ProcessingComponent = CreateDefaultSubobject<UCProcessingComponent>(TEXT("ProcessingComponent"));

	InfoUIType = EInfoUIType::Processor;
}

void ACProductionEquipment_Processor::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* game = GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	commuSubsystem_UI->GetOnProcessingTimeChangeRequestedDel().AddUObject(this, &ACProductionEquipment_Processor::OnProcessingTimeChangeRequested);
	commuSubsystem_UI->GetOnProcessingTimeChangeEnded().AddUObject(this, &ACProductionEquipment_Processor::OnProcessingTimeChangeEnded);
	commuSubsystem_UI->GetOnSimulationStateChangedDel().AddUObject(this, &ACProductionEquipment_Processor::OnSimulationStateChanged);

	PrevProcessingTime = ProcessingTime;

	UWorld* world = GetWorld();
	CheckNotValid(world);

	UCProductionStatSubsystem* proStatSubsystem = world->GetSubsystem<UCProductionStatSubsystem>();
	if (IsValid(proStatSubsystem))
		proStatSubsystem->RegisterEquipment(this);
}

void ACProductionEquipment_Processor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameInstance* game = GetGameInstance();
	if (game)
	{
		UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
		if (commuSubsystem_UI)
		{
			commuSubsystem_UI->GetOnProcessingTimeChangeRequestedDel().RemoveAll(this);
			commuSubsystem_UI->GetOnProcessingTimeChangeEnded().RemoveAll(this);
			commuSubsystem_UI->GetOnSimulationStateChangedDel().RemoveAll(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ACProductionEquipment_Processor::ReceiveProduct(const FProductData& InProductData)
{
	CheckNotValid(ProcessingComponent);
	if (ProcessingComponent->GetEquipmentState() != EEquipmentState::Idle) return;
	CheckFalse(ProcessingComponent->GetRequiredProducts().Contains(InProductData.ProductType));

	ArrivedProducts.FindOrAdd(InProductData.ProductType).Add(InProductData);
	
	if (!ProcessingComponent->StartProcessing(ArrivedProducts, GetInstancingMesh(), HISMInstanceIndex))
	{
		UITargetBroadcastInfo();
		return;
	}

	UWorld* world = GetWorld();
	CheckNotValid(world);

	ProcessingStartTime = world->GetTimeSeconds();
	UITargetBroadcastInfo();

	UCProductionStatSubsystem* proStatSubsystem = world->GetSubsystem<UCProductionStatSubsystem>();
	if (IsValid(proStatSubsystem))
		proStatSubsystem->NotifyEquipmentProcessingStateChanged(this, true);

	world->GetTimerManager().SetTimer(
		ProcessingHandle,
		this,
		&ACProductionEquipment_Processor::OnProcessingComplete,
		ProcessingTime,
		false
	);

	world->GetTimerManager().SetTimer(
		ProgressHandle,
		this,
		&ACProductionEquipment_Processor::OnProgressTick,
		0.1f,
		true,
		0.1f
	);
}

void ACProductionEquipment_Processor::OnProcessingComplete()
{
	if (PendingProcessingTime > 0.0f)
	{
		ProcessingTime = PendingProcessingTime;
		PendingProcessingTime = -1.0f;

		if (PrevProcessingTime != ProcessingTime)
		{
			FString logText = FString::Printf(TEXT("%s 가공 시간 %.1fs → %.1fs"), 
				*EquipmentID, PrevProcessingTime, ProcessingTime);
			SendLogMessage(ELogEventType::Alert, logText);
			
			PrevProcessingTime = ProcessingTime;
		}
	}

	UWorld* world = GetWorld();
	CheckNotValid(world);
	world->GetTimerManager().ClearTimer(ProgressHandle);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>();
	CheckNotValid(ioSubsystem);

	FProductData processedProduct = ProcessingComponent->CompleteProcessing(ArrivedProducts, GetInstancingMesh(), HISMInstanceIndex);
	ioSubsystem->BroadcastOnProductStarted(this, processedProduct);

	UITargetBroadcastInfo();

	UCProductionStatSubsystem* proStatSubsystem = world->GetSubsystem<UCProductionStatSubsystem>();
	if (IsValid(proStatSubsystem))
		proStatSubsystem->NotifyEquipmentProcessingStateChanged(this, false);
}

void ACProductionEquipment_Processor::OnProgressTick()
{
	UGameInstance* game = GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	const AActor* uiTarget = commuSubsystem_UI->GetCurrentUITarget();
	CheckFalse(uiTarget == this);

	UWorld* world = GetWorld();
	CheckNotValid(world);

	float elapsed = world->GetTimeSeconds() - ProcessingStartTime;
	float progress = FMath::Clamp(elapsed / ProcessingTime, 0.0f, 1.0f);
	commuSubsystem_UI->BroadcastOnProcessorProgressUpdated(progress);
}

void ACProductionEquipment_Processor::OnProcessingTimeChangeRequested(UClass* InProcessorClass, float InProcessingTime)
{
	if (GetClass() != InProcessorClass) return;

	if (ProcessingComponent->GetEquipmentState() == EEquipmentState::Processing)
	{
		PendingProcessingTime = InProcessingTime;
		return;
	}

	ProcessingTime = InProcessingTime;
}

void ACProductionEquipment_Processor::OnProcessingTimeChangeEnded()
{
	UGameInstance* game = GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	const AActor* uiTarget = commuSubsystem_UI->GetCurrentUITarget();

	if (uiTarget != this) return;
	if (PrevProcessingTime == ProcessingTime) return;

	FString logText = FString::Printf(TEXT("%s 가공 시간 %.1fs → %.1fs"),
		*EquipmentID, PrevProcessingTime, ProcessingTime);
	SendLogMessage(ELogEventType::Alert, logText);

	PrevProcessingTime = ProcessingTime;
}

void ACProductionEquipment_Processor::OnSimulationStateChanged(bool InIsRunning)
{
	if (ProcessingComponent->GetEquipmentState() != EEquipmentState::Processing) return;

	UWorld* world = GetWorld();
	CheckNotValid(world);

	FTimerManager& manager = world->GetTimerManager();

	if (InIsRunning)
	{
		ProcessingStartTime = world->GetTimeSeconds() - PausedProcessingTime;
		PausedProcessingTime = 0.0f;

		if (manager.IsTimerPaused(ProcessingHandle))
			manager.UnPauseTimer(ProcessingHandle);

		if (manager.IsTimerPaused(ProgressHandle))
			manager.UnPauseTimer(ProgressHandle);
	}

	else
	{
		PausedProcessingTime = world->GetTimeSeconds() - ProcessingStartTime;

		if (manager.IsTimerActive(ProcessingHandle))
			manager.PauseTimer(ProcessingHandle);

		if (manager.IsTimerActive(ProgressHandle))
			manager.PauseTimer(ProgressHandle);
	}
}

void ACProductionEquipment_Processor::BroadcastInfo()
{
	FProcessorInfoData infoData = ProcessingComponent->GetProcessorInfoData();
	infoData.ProcessingTime = ProcessingTime;
	
	UWorld* world = GetWorld();
	CheckNotValid(world);

	if (ProcessingComponent->GetEquipmentState() == EEquipmentState::Processing)
	{
		float elapsed;
		if (FMath::IsNearlyZero(PausedProcessingTime))
			elapsed = world->GetTimeSeconds() - ProcessingStartTime;
		else elapsed = PausedProcessingTime;
			
		infoData.Progress = FMath::Clamp(elapsed / ProcessingTime, 0.0f, 1.0f);
	}
	else infoData.Progress = 0.0f;
	
	for (const auto& pair : ArrivedProducts)
		infoData.ArrivedCount.Add(pair.Key, pair.Value.Num());

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	commuSubsystem_UI->BroadcastOnProcessorInfoUpdated(infoData);
}
