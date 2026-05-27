#include "ProductionEquipment/ProductionEquipment_Base/CProductionEquipment_Storage.h"
#include "Global.h"

#include "ProductionStat/CProductionStatSubsystem.h"

void ACProductionEquipment_Storage::BeginPlay()
{
	Super::BeginPlay();

	StoredProducts.Append(InitialProducts);	
}

void ACProductionEquipment_Storage::ReceiveProduct(const FProductData& InProductData)
{
	if (IsFull())
	{
		FLog::Log(FString::Printf(TEXT("Storage %s is FULL. Product dropped."), *GetName()));
		return;
	}

	StoredProducts.Add(InProductData);
	FLog::Log(FString::Printf(TEXT("Storage %s received. Stored: %d"), *GetName(), StoredProducts.Num()));

	UWorld* world = GetWorld();
	CheckNotValid(world);
	
	UCProductionStatSubsystem* proStatSubsystem = world->GetSubsystem<UCProductionStatSubsystem>();
	CheckNotValid(proStatSubsystem);

	proStatSubsystem->ReceiveFinalProduct();
}
