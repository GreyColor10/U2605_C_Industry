#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Types/CFacilityType.h"
#include "CStructDatas.generated.h" 

USTRUCT(BlueprintType)
struct FProductData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EProductType ProductType = EProductType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ItemName = TEXT("Default_Data");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MoveSpeed = 50.0f;

	UPROPERTY()
	float CurrentDistance = 0.0f;

	UPROPERTY()
	bool bArrived = false;

	UPROPERTY()
	int32 ProcessStage = 0;
};

USTRUCT(BlueprintType)
struct FStorageInfoData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EProductType ProductType = EProductType::None;

	UPROPERTY(BlueprintReadOnly)
	int32 StoredCount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxCapacity = 0;
};

USTRUCT(BlueprintType)
struct FProcessorInfoData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EEquipmentState State = EEquipmentState::Idle;

	UPROPERTY(BlueprintReadOnly)
	float ProcessingTime = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float ProcessingEndTime = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	TMap<EProductType, int32> RequiredProducts;

	UPROPERTY(BlueprintReadOnly)
	TMap<EProductType, int32> ArrivedCount;

	UPROPERTY(BlueprintReadOnly)
	EProductType ProducedProduct = EProductType::None;
};