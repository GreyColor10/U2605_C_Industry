#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StructData/CStructDatas.h"
#include "CUserWidget_Info_Processor.generated.h"

UCLASS(Abstract)
class U2605_C_API UCUserWidget_Info_Processor : public UUserWidget
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void OnProcessorInfoUpdatedInternal(const FProcessorInfoData& InProcessorInfoData);

	UFUNCTION()
	void OnProcessorProgressUpdatedInternal(float InProgress);

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnProcessorInfoUpdated(const FProcessorInfoData& InProcessorInfoData);

	UFUNCTION(BlueprintImplementableEvent)
	void OnProgressUpdated(float InProgress);

	UFUNCTION(BlueprintCallable)
	void RequestProcessingTimeChange(float InProcessingTime);

	UFUNCTION(BlueprintCallable)
	void RequestProcessingTimeChangeEnd();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	void ResetState();

private:
	bool bIsProcessing = false;
};
