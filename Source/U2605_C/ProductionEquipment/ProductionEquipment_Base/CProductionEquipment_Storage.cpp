#include "ProductionEquipment/ProductionEquipment_Base/CProductionEquipment_Storage.h"
#include "Global.h"

#include "ProductionStat/CProductionStatSubsystem.h"
#include "Communication/CCommunicationSubsystem_UI.h"
#include "Communication/CCommunicationSubsystem_IO.h"

ACProductionEquipment_Storage::ACProductionEquipment_Storage()
{
	InfoUIType = EInfoUIType::Storage;
}

void ACProductionEquipment_Storage::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < InitialProducts.Count; i++)
	{
		FProductData data;
		data.ProductType = InitialProducts.ProductType;
		StoredProducts.Add(data);
	}	
}

bool ACProductionEquipment_Storage::ReceiveProduct(const FProductData& InProductData)
{
	if (IsFull())
		return false;

	StoredProducts.Add(InProductData);

	UITargetBroadcastInfo();

	UWorld* world = GetWorld();
	CheckNotValidResult(world, false);
	
	UCProductionStatSubsystem* proStatSubsystem = world->GetSubsystem<UCProductionStatSubsystem>();
	CheckNotValidResult(proStatSubsystem, false);

	if (InProductData.ProductType == EProductType::CreamBread)
		proStatSubsystem->ReceiveFinalProduct();

	return true;
}

bool ACProductionEquipment_Storage::IsFull() const
{
	if (MaxCapacity <= 0) return false;
	return StoredProducts.Num() >= MaxCapacity;
}

void ACProductionEquipment_Storage::ShipProduct()
{
	CheckTrue(StoredProducts.IsEmpty());

	UGameInstance* game = GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>();
	CheckNotValid(ioSubsystem);

	FProductData productToShip = StoredProducts[0];
	StoredProducts.RemoveAt(0, 1, EAllowShrinking::No);

	productToShip.CurrentDistance = -GridConstants::HalfGridSize;
	productToShip.bArrived = false;

	ioSubsystem->BroadcastOnProductStarted(this, productToShip);
	UITargetBroadcastInfo();
}

void ACProductionEquipment_Storage::OnAutoShipTick()
{
	if (StoredProducts.IsEmpty())
	{
		FString logText = FString::Printf(TEXT("%s 재고 소진"), *EquipmentID);
		SendLogMessage(ELogEventType::Warning, logText);
		
		StopAutoShip();
		return;
	}

	if (IsShipBlocked())
	{
		if (!IsBlockLogSended)
		{
			FString logText = FString::Printf(TEXT("%s 입구 정체"), *EquipmentID);
			SendLogMessage(ELogEventType::Warning, logText);
			IsBlockLogSended = true;
		}
		
		return;
	}

	ShipProduct();
}

void ACProductionEquipment_Storage::StartAutoShip()
{
	CheckTrue(AutoShipInterval <= 0.0f);

	UWorld* world = GetWorld();
	CheckNotValid(world);

	FTimerDelegate del;
	del.BindUObject(this, &ACProductionEquipment_Storage::OnAutoShipTick);
	world->GetTimerManager().SetTimer(AutoShipHandle, del, AutoShipInterval, true, 0.0f);
}

void ACProductionEquipment_Storage::PauseAutoShip()
{
	UWorld* world = GetWorld();
	CheckNotValid(world);

	world->GetTimerManager().PauseTimer(AutoShipHandle);
}

void ACProductionEquipment_Storage::ResumeAutoShip()
{
	UWorld* world = GetWorld();
	CheckNotValid(world);

	world->GetTimerManager().UnPauseTimer(AutoShipHandle);
}

void ACProductionEquipment_Storage::StopAutoShip()
{
	UWorld* world = GetWorld();
	CheckNotValid(world);

	world->GetTimerManager().ClearTimer(AutoShipHandle);
}

void ACProductionEquipment_Storage::BroadcastInfo()
{
	UGameInstance* game = GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(uiSubsystem);

	FStorageInfoData infoData;
	infoData.ProductType = StoredProducts.IsEmpty() ? EProductType::None : StoredProducts[0].ProductType;
	infoData.StoredCount = StoredProducts.Num();
	infoData.MaxCapacity = MaxCapacity;

	uiSubsystem->BroadcastOnStorageInfoUpdated(infoData);
}

void ACProductionEquipment_Storage::OnSimulationStateChanged(bool InIsRunning)
{
	if (InIsRunning)
	{
		UWorld* world = GetWorld();
		CheckNotValid(world);

		if (world->GetTimerManager().IsTimerPaused(AutoShipHandle)) ResumeAutoShip();
		else StartAutoShip();
	}

	else PauseAutoShip();
}