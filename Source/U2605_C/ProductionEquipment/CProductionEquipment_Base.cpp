#include "ProductionEquipment/CProductionEquipment_Base.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"

#include "MeshInstancing/CInstancedMeshSubsystem.h"
#include "Conveyor/CConveyorSubsystem.h"
#include "Communication/CCommunicationSubsystem_UI.h"

bool ACProductionEquipment_Base::IsFull() const
{
	if (MaxCapacity <= 0) return false;
	return StoredProducts.Num() >= MaxCapacity;
}

ACProductionEquipment_Base::ACProductionEquipment_Base()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);
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
}

void ACProductionEquipment_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* world = GetWorld();
	if (IsValid(world))
	{
		if (UCConveyorSubsystem* conveyorSubsystem = world->GetSubsystem<UCConveyorSubsystem>())
		{
			conveyorSubsystem->UnregisterSink(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ACProductionEquipment_Base::UITargetBroadcastInfo()
{
	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	const AActor* uiTarget = commuSubsystem_UI->BroadcastOnOnUITargetGotten();
	if (uiTarget == this) BroadcastInfo();
}

void ACProductionEquipment_Base::OnClicked(const FHitResult& InHit)
{
	BroadcastInfo();
}
