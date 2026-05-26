#include "ProductionEquipment/ProductionEquipment_Base/CProductionEquipment_Storage.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_IO.h"
#include "Conveyor/CConveyorSubsystem.h"

void ACProductionEquipment_Storage::BeginPlay()
{
	Super::BeginPlay();

	// 초기 상품을 보관 목록에 채움
	StoredProducts.Append(InitialProducts);

	UWorld* world = GetWorld();
	CheckNotValid(world);

	// Conveyor Graph에 Sink로 등록 (입고 위치 매칭용)
	UCConveyorSubsystem* conveyorSubsystem = world->GetSubsystem<UCConveyorSubsystem>();
	CheckNotValid(conveyorSubsystem);
	conveyorSubsystem->RegisterSink(this);

	// IO 도착 이벤트 구독 (입고)
	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>();
	CheckNotValid(ioSubsystem);

	ioSubsystem->GetProductDeliveredDel().AddUObject(this, &ACProductionEquipment_Storage::OnProductDelivered);
}

void ACProductionEquipment_Storage::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* world = GetWorld();
	if (IsValid(world))
	{
		if (UCConveyorSubsystem* conveyorSubsystem = world->GetSubsystem<UCConveyorSubsystem>())
		{
			conveyorSubsystem->UnregisterSink(this);
		}

		if (UGameInstance* game = world->GetGameInstance())
		{
			if (UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>())
				ioSubsystem->GetProductDeliveredDel().RemoveAll(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

bool ACProductionEquipment_Storage::ShipProduct()
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
	return true;
}

bool ACProductionEquipment_Storage::IsFull() const
{
	if (MaxCapacity <= 0) return false;
	return StoredProducts.Num() >= MaxCapacity;
}

void ACProductionEquipment_Storage::OnProductDelivered(AActor* InTargetStorage, const FProductData& InProductData)
{
	CheckFalse(InTargetStorage == this);

	if (IsFull())
	{
		FLog::Log(FString::Printf(TEXT("Storage %s is FULL. Product dropped."), *GetName()));
		return;
	}

	StoredProducts.Add(InProductData);
	FLog::Log(FString::Printf(TEXT("Storage %s received. Stored: %d"), *GetName(), StoredProducts.Num()));
}
