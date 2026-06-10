#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StructData/CStructDatas.h"
#include "CScreenWidgetComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class U2605_C_API UCScreenWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UCUserWidget_Screen> UI_ScreenClass;

	UPROPERTY()
	TObjectPtr<UCUserWidget_Screen> UI_Player = nullptr;

private:
	UFUNCTION()
	void OnStoredFinalProductUpdated(int InStoredFinalProductNum);

	UFUNCTION()
	void OnSimulationStateChanged(bool InIsRunning);

public:	
	UCScreenWidgetComponent();

public:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
