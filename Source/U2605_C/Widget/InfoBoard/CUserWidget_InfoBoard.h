#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StructData/CStructDatas.h"
#include "CUserWidget_InfoBoard.generated.h"

UCLASS()
class U2605_C_API UCUserWidget_InfoBoard : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateDashboard(const FDashboardData& InDashboardData);
};
