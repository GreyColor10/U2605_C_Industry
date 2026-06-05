#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CInfoUIActor.generated.h"

UCLASS(Abstract)
class U2605_C_API ACInfoUIActor : public AActor
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UCUserWidget_Info_Storage> StorageWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UCUserWidget_Info_Processor> ProcessorWidgetClass;

private:
    UPROPERTY()
    TObjectPtr<UCUserWidget_Info_Storage> StorageWidget;

    UPROPERTY()
    TObjectPtr<UCUserWidget_Info_Processor> ProcessorWidget;

    TWeakObjectPtr<AActor> TargetActor = nullptr;

    UPROPERTY()
    TObjectPtr<class UUserWidget> ActiveWidget = nullptr;

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void Hide();

public:
    ACInfoUIActor();

protected:
    void BeginPlay() override;
    void Tick(float DeltaTime) override;

public:
    void SetTarget(AActor* InActor);
    
    const AActor* GetTarget() const;

private:
    void UpdateWidgetPosition();
};
