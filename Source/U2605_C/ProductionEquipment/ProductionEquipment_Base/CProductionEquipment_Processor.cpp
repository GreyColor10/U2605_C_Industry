#include "ProductionEquipment/ProductionEquipment_Base/CProductionEquipment_Processor.h"
#include "Global.h"
#include "Communication/CCommunicationSubsystem_IO.h"

void ACProductionEquipment_Processor::ReceiveProduct(const FProductData& InProductData)
{
	CheckFalse(State == EEquipmentState::Idle);

	CheckFalse(RequiredProducts.Contains(InProductData.ItemID));

	ArrivedProducts.FindOrAdd(InProductData.ItemID).Add(InProductData);
	FLog::Log(FString::Printf(TEXT("Processor %s received ItemID %d"), *GetName(), InProductData.ItemID));

	if (!CanStartProcessing()) return;

	for (auto& pair : RequiredProducts)
	{
		TArray<FProductData>& arrived = ArrivedProducts[pair.Key];
		arrived.RemoveAt(0, pair.Value, EAllowShrinking::No);
	}

	State = EEquipmentState::Processing;

	UWorld* world = GetWorld();
	CheckNotValid(world);

	world->GetTimerManager().SetTimer(
		ProcessingHandle,
		this,
		&ACProductionEquipment_Processor::OnProcessingComplete,
		ProcessingTime,
		false
	);
}

void ACProductionEquipment_Processor::OnProcessingComplete()
{
	State = EEquipmentState::Idle;

	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>();
	CheckNotValid(ioSubsystem);

	// 처리 완료된 상품 출고 — 임시로 빈 FProductData (Day 5에서 ProcessStage 추가 예정)
	FProductData processedProduct;
	processedProduct.CurrentDistance = 0.0f;
	processedProduct.bArrived = false;

	ioSubsystem->BroadcastOnProductStarted(this, processedProduct);

	FLog::Log(FString::Printf(TEXT("Processor %s completed. Shipped product."), *GetName()));
}

bool ACProductionEquipment_Processor::CanStartProcessing() const
{
	for (const auto& pair : RequiredProducts)
	{
		const TArray<FProductData>* arrived = ArrivedProducts.Find(pair.Key);
		if (!arrived) return false;
		if (arrived->Num() < pair.Value) return false;
	}
	return true;
}
