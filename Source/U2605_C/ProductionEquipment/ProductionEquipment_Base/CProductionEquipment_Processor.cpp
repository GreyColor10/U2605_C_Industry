#include "ProductionEquipment/ProductionEquipment_Base/CProductionEquipment_Processor.h"
#include "Global.h"

#include "Component/ActorComponent/CProcessingComponent.h"
#include "Communication/CCommunicationSubsystem_IO.h"
#include "Communication/CCommunicationSubsystem_UI.h"
#include "ProductionStat/CProductionStatSubsystem.h"
#include "CScenarioSubsystem.h"

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

	UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	if (IsValid(uiSubsystem))
	{
		uiSubsystem->GetOnProcessingTimeChangeRequestedDel().AddUObject(this, &ACProductionEquipment_Processor::OnProcessingTimeChangeRequested);
		uiSubsystem->GetOnProcessingTimeChangeEnded().AddUObject(this, &ACProductionEquipment_Processor::OnProcessingTimeChangeEnded);
	}

	PrevProcessingTime = ProcessingTime;

	UWorld* world = GetWorld();
	CheckNotValid(world);

	UCScenarioSubsystem* scenarioSubsystem = world->GetSubsystem<UCScenarioSubsystem>();
	if (IsValid(scenarioSubsystem))
		scenarioSubsystem->GetOnShortageScenarioActiveChanged().AddUObject(this, &ACProductionEquipment_Processor::OnShortageScenarioActiveChanged);

	UCProductionStatSubsystem* proStatSubsystem = world->GetSubsystem<UCProductionStatSubsystem>();
	if (IsValid(proStatSubsystem))
		proStatSubsystem->RegisterEquipment(this);
}

void ACProductionEquipment_Processor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* world = GetWorld();
	if (IsValid(world))
	{
		UCScenarioSubsystem* scenarioSubsystem = world->GetSubsystem<UCScenarioSubsystem>();
		if (IsValid(scenarioSubsystem))
			scenarioSubsystem->GetOnShortageScenarioActiveChanged().RemoveAll(this);

		UGameInstance* game = world->GetGameInstance();
		if (IsValid(game))
		{
			UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
			if (IsValid(uiSubsystem))
			{
				uiSubsystem->GetOnProcessingTimeChangeRequestedDel().RemoveAll(this);
				uiSubsystem->GetOnProcessingTimeChangeEnded().RemoveAll(this);
			}
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

bool ACProductionEquipment_Processor::ReceiveProduct(const FProductData& InProductData)
{
	CheckNotValidResult(ProcessingComponent, false);

	const TMap<EProductType, int32>& required = ProcessingComponent->GetRequiredProducts();
	CheckFalseResult(required.Contains(InProductData.ProductType), false);
	
	int32 requiredCount = required[InProductData.ProductType];
	int32 currentCount = ArrivedProducts.Contains(InProductData.ProductType)
		? ArrivedProducts[InProductData.ProductType].Num()
		: 0;

	if (currentCount >= requiredCount * BufferMultiplier)
		return false;

	ArrivedProducts.FindOrAdd(InProductData.ProductType).Add(InProductData);
	
	if (!TryStartProcessing()) UITargetBroadcastInfo();
	
	return true;
}

bool ACProductionEquipment_Processor::TryStartProcessing()
{
	CheckNotValidResult(ProcessingComponent, false);

	CheckFalseResult(ProcessingComponent->IsIdleState(), false);
	CheckFalseResult(ProcessingComponent->StartProcessing(ArrivedProducts, GetInstancingMesh(), HISMInstanceIndex), false);
		
	UWorld* world = GetWorld();
	CheckNotValidResult(world, false);

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

	return true;
}

void ACProductionEquipment_Processor::OnProcessingComplete()
{
	if (PendingProcessingTime > 0.0f)
	{
		ProcessingTime = PendingProcessingTime;
		PendingProcessingTime = -1.0f;

		if (PrevProcessingTime != ProcessingTime)
		{
			UWorld* world = GetWorld();
			CheckNotValid(world);

			FString logText = FString::Printf(TEXT("%s 가공 시간 %.1fs → %.1fs"), 
				*EquipmentID, PrevProcessingTime, ProcessingTime);
			SendLogMessage(world ,ELogEventType::Alert, logText);
			
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
	
	UCProductionStatSubsystem* proStatSubsystem = world->GetSubsystem<UCProductionStatSubsystem>();
	if (IsValid(proStatSubsystem))
		proStatSubsystem->NotifyEquipmentProcessingStateChanged(this, false);

	if (!TryStartProcessing()) UITargetBroadcastInfo();
}

void ACProductionEquipment_Processor::OnProgressTick()
{
	UGameInstance* game = GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(uiSubsystem);

	const AActor* uiTarget = uiSubsystem->GetCurrentUITarget();
	CheckFalse(uiTarget == this);

	UWorld* world = GetWorld();
	CheckNotValid(world);

	float elapsed = world->GetTimeSeconds() - ProcessingStartTime;
	float progress = FMath::Clamp(elapsed / ProcessingTime, 0.0f, 1.0f);
	uiSubsystem->BroadcastOnProcessorProgressUpdated(progress);
}

void ACProductionEquipment_Processor::OnProcessingTimeChangeRequested(UClass* InProcessorClass, float InProcessingTime)
{
	if (GetClass() != InProcessorClass) return;

	if (ProcessingComponent->IsProcessingState())
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

	UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(uiSubsystem);

	const AActor* uiTarget = uiSubsystem->GetCurrentUITarget();

	if (uiTarget != this) return;
	if (PrevProcessingTime == ProcessingTime) return;

	UWorld* world = GetWorld();
	CheckNotValid(world);

	FString logText = FString::Printf(TEXT("%s 가공 시간 %.1fs → %.1fs"),
		*EquipmentID, PrevProcessingTime, ProcessingTime);
	SendLogMessage(world ,ELogEventType::Alert, logText);

	PrevProcessingTime = ProcessingTime;
}

void ACProductionEquipment_Processor::BroadcastInfo()
{
	FProcessorInfoData infoData = ProcessingComponent->GetProcessorInfoData();
	infoData.ProcessingTime = ProcessingTime;
	
	UWorld* world = GetWorld();
	CheckNotValid(world);

	if (ProcessingComponent->IsProcessingState())
	{
		float elapsed;
		if (FMath::IsNearlyZero(PausedProcessingTime))
			elapsed = world->GetTimeSeconds() - ProcessingStartTime;
		else elapsed = PausedProcessingTime;
			
		infoData.Progress = FMath::Clamp(elapsed / ProcessingTime, 0.0f, 1.0f);

		if (PendingProcessingTime > 0.0f)
			infoData.ProcessingTime = PendingProcessingTime;
	}
	else infoData.Progress = 0.0f;
	
	for (const auto& pair : ArrivedProducts)
		infoData.ArrivedCount.Add(pair.Key, pair.Value.Num());

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(uiSubsystem);

	uiSubsystem->BroadcastOnProcessorInfoUpdated(infoData);
}

void ACProductionEquipment_Processor::OnSimulationStateChanged(bool InIsRunning)
{
	CheckFalse(ProcessingComponent->IsProcessingState());

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

void ACProductionEquipment_Processor::OnShortageScenarioActiveChanged(bool IsActive)
{
	float scenarioProcessingTime;

	if (IsActive) scenarioProcessingTime = ProcessingTime * 2.0f;
	else scenarioProcessingTime = ProcessingTime * 0.5f;

	if (ProcessingComponent->IsProcessingState())
		PendingProcessingTime = scenarioProcessingTime;
	else ProcessingTime = scenarioProcessingTime;

	UITargetBroadcastInfo();
}
