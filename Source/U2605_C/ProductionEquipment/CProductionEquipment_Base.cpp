#include "ProductionEquipment/CProductionEquipment_Base.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"

#include "MeshInstancing/CInstancedMeshSubsystem.h"
#include "Conveyor/CConveyorSubsystem.h"
#include "Communication/CCommunicationSubsystem_IO.h"
#include "Communication/CCommunicationSubsystem_UI.h"
#include "Component/ActorComponent/CLogComponent.h"
#include "SimulationTime/CSimulationTimeSubsystem.h"

ACProductionEquipment_Base::ACProductionEquipment_Base()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	LogComponent = CreateDefaultSubobject<UCLogComponent>(TEXT("LogComponent"));
}

void ACProductionEquipment_Base::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* world = GetWorld();
	CheckNotValid(world);

	UCInstancedMeshSubsystem* instancingSubsystem = world->GetSubsystem<UCInstancedMeshSubsystem>();
	CheckNotValid(instancingSubsystem);
	CheckNull(InstancingMesh);
	HISMInstanceIndex = instancingSubsystem->RegisterFixedFacility(InstancingMesh, GetActorTransform());

	// Conveyor Graph에 Sink로 등록 (입고 위치 매칭용)
	UCConveyorSubsystem* conveyorSubsystem = world->GetSubsystem<UCConveyorSubsystem>();
	CheckNotValid(conveyorSubsystem);
	conveyorSubsystem->RegisterSink(this);	

	UCSimulationTimeSubsystem* timeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
	CheckNotValid(timeSubsystem);

	timeSubsystem->GetOnSimulationStateChangedDel().AddUObject(this, &ACProductionEquipment_Base::OnSimulationStateChanged);
}

void ACProductionEquipment_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* world = GetWorld();
	if (IsValid(world))
	{
		UCConveyorSubsystem* conveyorSubsystem = world->GetSubsystem<UCConveyorSubsystem>();
		if (IsValid(conveyorSubsystem))
			conveyorSubsystem->UnregisterSink(this);

		UCSimulationTimeSubsystem* timeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
		if (IsValid(timeSubsystem))
			timeSubsystem->GetOnSimulationStateChangedDel().RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ACProductionEquipment_Base::UITargetBroadcastInfo()
{
	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(uiSubsystem);

	const AActor* uiTarget = uiSubsystem->GetCurrentUITarget();
	if (uiTarget == this) BroadcastInfo();
}

void ACProductionEquipment_Base::SendLogMessage(ELogEventType InEventType, FString InLogMessage)
{
	LogComponent->SendLogMessage(InEventType, InLogMessage);
}

const bool ACProductionEquipment_Base::IsShipBlocked() const
{
	UWorld* world = GetWorld();
	CheckNotValidResult(world, false);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValidResult(game, false);

	UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>();
	CheckNotValidResult(ioSubsystem, false);

	return ioSubsystem->IsShipBlocked(GetActorLocation());
}

void ACProductionEquipment_Base::OnClicked(const FHitResult& InHit)
{
	BroadcastInfo();
}
