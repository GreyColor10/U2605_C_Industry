#include "Component/ActorComponent/CProcessingComponent.h"
#include "Global.h"

#include "MeshInstancing/CInstancedMeshSubsystem.h"
#include "ProductionStat/CProductionStatSubsystem.h"

UCProcessingComponent::UCProcessingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UCProcessingComponent::StartProcessing(const TMap<EProductType, TArray<FProductData>>& InArrivedProducts, UStaticMesh* InStaticMesh, int32 InMeshIndex)
{
	if (!CanStartProcessing(InArrivedProducts)) return false;

	UWorld* world = GetWorld();
	CheckNotValidResult(world, false);

	State = EEquipmentState::Processing;

	UCInstancedMeshSubsystem* instancingSubsystem = world->GetSubsystem<UCInstancedMeshSubsystem>();
	CheckNotValidResult(instancingSubsystem, false);

	instancingSubsystem->SetCustomData(InStaticMesh, InMeshIndex, 0, 1.0f);

	return true;
}

FProductData UCProcessingComponent::CompleteProcessing(TMap<EProductType, TArray<FProductData>>& InArrivedProducts, UStaticMesh* InStaticMesh, int32 InMeshIndex)
{
	State = EEquipmentState::Idle;

	for (auto& pair : RequiredProducts)
	{
		TArray<FProductData>& arrived = InArrivedProducts[pair.Key];
		arrived.RemoveAt(0, pair.Value, EAllowShrinking::No);
	}

	FProductData processedProduct;

	UWorld* world = GetWorld();
	CheckNotValidResult(world, processedProduct);

	UCProductionStatSubsystem* proStatSubsystem = world->GetSubsystem<UCProductionStatSubsystem>();
	if (IsValid(proStatSubsystem))
		proStatSubsystem->ReceiveIntermediateProduct(ProducedProducts);

	UCInstancedMeshSubsystem* instancingSubsystem = world->GetSubsystem<UCInstancedMeshSubsystem>();
	CheckNotValidResult(instancingSubsystem, processedProduct);
	instancingSubsystem->SetCustomData(InStaticMesh, InMeshIndex, 0, 0.0f);

	processedProduct.CurrentDistance = 0.0f;
	processedProduct.bArrived = false;
	processedProduct.ProductType = ProducedProducts;
	processedProduct.ProcessStage = (int)ProducedProducts - 1;

	return processedProduct;
}

FProcessorInfoData UCProcessingComponent::GetProcessorInfoData() const
{
	FProcessorInfoData infoData;
	infoData.State = State;
	infoData.RequiredProducts = RequiredProducts;
	infoData.ProducedProduct = ProducedProducts;

	return infoData;
}

bool UCProcessingComponent::CanStartProcessing(const TMap<EProductType, TArray<FProductData>>& InArrivedProducts) const
{
	for (const auto& pair : RequiredProducts)
	{
		const TArray<FProductData>* arrived = InArrivedProducts.Find(pair.Key);
		if (!arrived) return false;
		if (arrived->Num() < pair.Value) return false;
	}
	return true;
}
