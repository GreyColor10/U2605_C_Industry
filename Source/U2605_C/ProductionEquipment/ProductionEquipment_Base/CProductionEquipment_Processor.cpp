#include "ProductionEquipment/ProductionEquipment_Base/CProductionEquipment_Processor.h"
#include "Global.h"

#include "Component/ActorComponent/CProcessingComponent.h"
#include "Communication/CCommunicationSubsystem_IO.h"
#include "Communication/CCommunicationSubsystem_UI.h"
#include "ProductionStat/CProductionStatSubsystem.h"

void ACProductionEquipment_Processor::OnProcessingTimeChangeRequested(UClass* InProcessorClass, float InProcessingTime)
{
	if (GetClass() != InProcessorClass) return;

	if (ProcessingComponent->GetEquipmentState() == EEquipmentState::Processing)
	{
		PendingProcessingTime = InProcessingTime;
		return;
	}

	ProcessingTime = InProcessingTime;

	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	UCProductionStatSubsystem* statSubsystem = world->GetSubsystem<UCProductionStatSubsystem>();
	CheckNotValid(statSubsystem);

	FLogEntry entry;
	entry.EventType = ELogEventType::Warning;
	entry.Message = FString::Printf(TEXT("%s 작동 시간 %.1f초로 변경"), *GetName(), InProcessingTime);
	entry.Timestamp = IsValid(statSubsystem)
		? world->GetTimeSeconds() - statSubsystem->GetSimulationStartTime()
		: 0.0f;

	commuSubsystem_UI->BroadcastOnLogEntryAdded(entry);
}

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

	commuSubsystem_UI->GetOnProcessingTimeChangeRequestedDel().AddDynamic(this, &ACProductionEquipment_Processor::OnProcessingTimeChangeRequested);
	commuSubsystem_UI->GetOnSimulationStateChangedDel().AddUObject(this, &ACProductionEquipment_Processor::OnSimulationStateChanged);
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
			commuSubsystem_UI->GetOnSimulationStateChangedDel().RemoveAll(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ACProductionEquipment_Processor::ReceiveProduct(const FProductData& InProductData)
{
	CheckNotValid(ProcessingComponent);
	CheckFalse(ProcessingComponent->GetEquipmentState() == EEquipmentState::Idle);
	CheckFalse(ProcessingComponent->GetRequiredProducts().Contains(InProductData.ProductType));

	ArrivedProducts.FindOrAdd(InProductData.ProductType).Add(InProductData);

	if (!ProcessingComponent->StartProcessing(ArrivedProducts, InstancingMesh, HISMInstanceIndex))
	{
		UITargetBroadcastInfo();
		return;
	}

	UWorld* world = GetWorld();
	CheckNotValid(world);

	ProcessingStartTime = world->GetTimeSeconds();
	UITargetBroadcastInfo();

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
	}

	UWorld* world = GetWorld();
	CheckNotValid(world);
	world->GetTimerManager().ClearTimer(ProgressHandle);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>();
	CheckNotValid(ioSubsystem);

	FProductData processedProduct = ProcessingComponent->CompleteProcessing(ArrivedProducts, InstancingMesh, HISMInstanceIndex);
	ioSubsystem->BroadcastOnProductStarted(this, processedProduct);

	UITargetBroadcastInfo();
}

void ACProductionEquipment_Processor::OnProgressTick()
{
	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	const AActor* uiTarget = commuSubsystem_UI->GetCurrentUITarget();
	CheckFalse(uiTarget == this);

	float elapsed = world->GetTimeSeconds() - ProcessingStartTime;
	float progress = FMath::Clamp(elapsed / ProcessingTime, 0.0f, 1.0f);
	commuSubsystem_UI->BroadcastOnProcessorProgressUpdated(progress);
}

void ACProductionEquipment_Processor::OnSimulationStateChanged(bool InIsRunning)
{
	UWorld* world = GetWorld();
	CheckNotValid(world);

	FTimerManager& manager = world->GetTimerManager();

	if (InIsRunning)
	{
		float pausedElapsed = PausedProcessingTime;
		ProcessingStartTime = world->GetTimeSeconds() - pausedElapsed;

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

	UITargetBroadcastInfo();
}

void ACProductionEquipment_Processor::BroadcastInfo()
{
	FProcessorInfoData infoData = ProcessingComponent->GetProcessorInfoData();
	infoData.ProcessingTime = ProcessingTime;
	
	UWorld* world = GetWorld();
	CheckNotValid(world);

	if (ProcessingComponent->GetEquipmentState() == EEquipmentState::Processing)
	{
		float elapsed = world->GetTimeSeconds() - ProcessingStartTime;
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
