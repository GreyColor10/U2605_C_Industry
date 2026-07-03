#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CUserWidget_SimulationPanel.generated.h"

UCLASS()
class U2605_C_API UCUserWidget_SimulationPanel : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UFUNCTION()
	void OnOnScenarioActiveChangedInternal(bool InIsActive, const FString& InScenarioName);

	UFUNCTION()
	void OnScenarioRemainingUpdatedInternal(float InRemainingSeconds);

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnScenarioActiveChanged(bool InIsActive, const FString& InScenarioName);

	UFUNCTION(BlueprintImplementableEvent)
	void OnScenarioRemainingUpdated(float InRemainingSeconds);

protected:
	void NativeConstruct() override;
	void NativeDestruct() override;
};
