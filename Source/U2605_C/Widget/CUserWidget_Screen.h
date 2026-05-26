#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StructData/CStructDatas.h"
#include "CUserWidget_Screen.generated.h"

UCLASS()
class U2605_C_API UCUserWidget_Screen : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnTotalProductCountUpdated(int InTotalProductNum);
};
