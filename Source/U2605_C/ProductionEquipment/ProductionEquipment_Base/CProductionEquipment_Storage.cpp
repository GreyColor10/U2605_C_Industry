#include "ProductionEquipment/ProductionEquipment_Base/CProductionEquipment_Storage.h"
#include "Global.h"

#include "ProductionStat/CProductionStatSubsystem.h"
#include "Communication/CCommunicationSubsystem_UI.h"
#include "Communication/CCommunicationSubsystem_IO.h"

bool ACProductionEquipment_Storage::ShipProduct()
{
	CheckFalseResult(StoredProducts.Num() > 0, false);

	UWorld* world = GetWorld();
	CheckNotValidResult(world, false);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValidResult(game, false);

	UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>();
	CheckNotValidResult(ioSubsystem, false);

	FProductData productToShip = StoredProducts[0];
	StoredProducts.RemoveAt(0, 1, EAllowShrinking::No);

	productToShip.CurrentDistance = 0.0f;
	productToShip.bArrived = false;

	ioSubsystem->BroadcastOnProductStarted(this, productToShip);
	UITargetBroadcastInfo();

	return true;
}

ACProductionEquipment_Storage::ACProductionEquipment_Storage()
{
	InfoUIType = EInfoUIType::Storage;
}

void ACProductionEquipment_Storage::BeginPlay()
{
	Super::BeginPlay();

	StoredProducts.Append(InitialProducts);	
}

void ACProductionEquipment_Storage::ReceiveProduct(const FProductData& InProductData)
{
	if (IsFull())
	{
		return;
	}

	StoredProducts.Add(InProductData);

	UITargetBroadcastInfo();

	UWorld* world = GetWorld();
	CheckNotValid(world);
	
	UCProductionStatSubsystem* proStatSubsystem = world->GetSubsystem<UCProductionStatSubsystem>();
	CheckNotValid(proStatSubsystem);

	if (InProductData.ProductType == EProductType::CreamBread)
		proStatSubsystem->ReceiveFinalProduct();
}

void ACProductionEquipment_Storage::BroadcastInfo()
{
	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	FStorageInfoData infoData;
	infoData.ProductType = StoredProducts.IsEmpty() ? EProductType::None : StoredProducts[0].ProductType;
	infoData.StoredCount = StoredProducts.Num();
	infoData.MaxCapacity = MaxCapacity;

	commuSubsystem_UI->BroadcastOnStorageInfoUpdated(infoData);
}
