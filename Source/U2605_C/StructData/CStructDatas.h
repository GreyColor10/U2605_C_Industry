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
	FString StorageName = TEXT("");

	UPROPERTY(BlueprintReadOnly)
	EProductType ProductType = EProductType::None;

	UPROPERTY(BlueprintReadOnly)
	int32 StoredCount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxCapacity = 0;
};