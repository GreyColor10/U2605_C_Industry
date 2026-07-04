#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StructData/CStructDatas.h"
#include "CUserWidget_ScenarioPanel.generated.h"

UCLASS()
class U2605_C_API UCUserWidget_ScenarioPanel : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UFUNCTION()
	void OnScenarioDeactivedInternal();

	UFUNCTION()
	void OnScenarioRemainingUpdatedInternal(float InRemainingSeconds);

	UFUNCTION()
	void OnScenarioComparisonReadyInternal(const FScenarioComparisonResult& InResult);

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnScenarioDeactived();

	UFUNCTION(BlueprintImplementableEvent)
	void OnScenarioRemainingUpdated(float InRemainingSeconds);

	UFUNCTION(BlueprintImplementableEvent)
	void OnScenarioComparisonReady(const FScenarioComparisonResult& InResult);

protected:
	void NativeConstruct() override;
	void NativeDestruct() override;
};
