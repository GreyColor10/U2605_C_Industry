#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CStructDatas.generated.h" 

USTRUCT(BlueprintType)
struct FProductData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ItemID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ItemName = TEXT("Default_Data");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MoveSpeed = 50.0f;

	UPROPERTY()
	float CurrentDistance = 0.0f;

	UPROPERTY()
	bool bArrived = false;

	/*UPROPERTY(BlueprintReadOnly)
	float value = 100.0f;*/
};
