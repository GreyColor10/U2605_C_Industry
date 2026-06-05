#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StructData/CStructDatas.h"
#include "CUserWidget_Info_Processor.generated.h"

UCLASS(Abstract)
class U2605_C_API UCUserWidget_Info_Processor : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	UFUNCTION()
	void OnProcessorInfoUpdatedInternal(const FProcessorInfoData& InProcessorInfoData);

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnProcessorInfoUpdated(const FProcessorInfoData& InProcessorInfoData);
};
