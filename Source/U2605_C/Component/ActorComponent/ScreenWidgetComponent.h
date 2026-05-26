#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StructData/CStructDatas.h"
#include "ScreenWidgetComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class U2605_C_API UScreenWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UCUserWidget_Screen> UI_ScreenClass;

	UPROPERTY()
	TObjectPtr<UCUserWidget_Screen> UI_Player = nullptr;

private:
	UFUNCTION()
	void OnShippable(FProductData InProductData);

public:	
	UScreenWidgetComponent();

public:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
