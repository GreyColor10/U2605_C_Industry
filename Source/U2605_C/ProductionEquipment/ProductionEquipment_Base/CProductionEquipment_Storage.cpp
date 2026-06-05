#include "ProductionEquipment/ProductionEquipment_Base/CProductionEquipment_Storage.h"
#include "Global.h"

#include "ProductionStat/CProductionStatSubsystem.h"
#include "Communication/CCommunicationSubsystem_UI.h"
#include "BaseSystem/U2605_CGameModeBase.h"
#include "Widget/InfoUI/CInfoUIActor.h"

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

	UITargetBroadcastStorageInfo();

	UWorld* world = GetWorld();
	CheckNotValid(world);
	
	UCProductionStatSubsystem* proStatSubsystem = world->GetSubsystem<UCProductionStatSubsystem>();
	CheckNotValid(proStatSubsystem);

	proStatSubsystem->ReceiveFinalProduct();
}

void ACProductionEquipment_Storage::BroadcastStorageInfo()
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

void ACProductionEquipment_Storage::UITargetBroadcastStorageInfo()
{
	UWorld* world = GetWorld();
	CheckNotValid(world);

	AGameModeBase* gameMode = world->GetAuthGameMode();
	CheckNotValid(gameMode);

	AU2605_CGameModeBase* cGameMode = Cast<AU2605_CGameModeBase>(gameMode);
	CheckNotValid(cGameMode);

	ACInfoUIActor* infoUI = cGameMode->GetInfoUIActor();
	CheckNotValid(infoUI);

	const AActor* uiTarget = infoUI->GetTarget();
	if (uiTarget == this) BroadcastStorageInfo();
}

void ACProductionEquipment_Storage::OnClicked(const FHitResult& InHit)
{
	BroadcastStorageInfo();
}

void ACProductionEquipment_Storage::OnAfterShipProduct()
{
	UITargetBroadcastStorageInfo();
}
