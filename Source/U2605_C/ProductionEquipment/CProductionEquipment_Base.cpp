#include "ProductionEquipment/CProductionEquipment_Base.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"

#include "MeshInstancing/CInstancedMeshSubsystem.h"
#include "Conveyor/CConveyorSubsystem.h"
#include "Communication/CCommunicationSubsystem_IO.h"

bool ACProductionEquipment_Base::ShipProduct()
{
	CheckFalseResult(StoredProducts.Num() > 0, false);

	UWorld* world = GetWorld();
	CheckNotValidResult(world, false);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValidResult(game, false);

	UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>();
	CheckNotValidResult(ioSubsystem, false);

	// FIFO 출고
	FProductData productToShip = StoredProducts[0];
	StoredProducts.RemoveAt(0, 1, EAllowShrinking::No);

	// 시뮬레이션 상태 초기화 (재고에 들어있는 동안 변형되었을 수 있음)
	productToShip.CurrentDistance = 0.0f;
	productToShip.bArrived = false;

	ioSubsystem->BroadcastOnProductStarted(this, productToShip);
	OnAfterShipProduct();
	return true;
}

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
