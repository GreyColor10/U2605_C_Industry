#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StructData/CStructDatas.h"
#include "CUserWidget_Screen.generated.h"

UCLASS()
class U2605_C_API UCUserWidget_Screen : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Finance")
	float InitialCapital = 10000.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Finance")
	float NetProfit = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Finance")
	float CurrentBalance = 10000.0f;

public:
	UFUNCTION(BlueprintNativeEvent)
	void OnProductShipAble(FProductData InProductData);
	void OnProductShipAble_Implementation(FProductData InProductData);
};
