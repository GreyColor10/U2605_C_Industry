#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CInfoUIActor.generated.h"

UCLASS(Abstract)
class U2605_C_API ACInfoUIActor : public AActor
{
	GENERATED_BODY()
	
private:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UCUserWidget_Info_Storage> StorageWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UCUserWidget_Info_Processor> ProcessorWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Outline")
    TObjectPtr<UMaterialInterface> OutlineMaterial;

private:
    UPROPERTY()
    TObjectPtr<UCUserWidget_Info_Storage> StorageWidget;

    UPROPERTY()
    TObjectPtr<UCUserWidget_Info_Processor> ProcessorWidget;

    TWeakObjectPtr<AActor> TargetActor = nullptr;

    UPROPERTY()
    TObjectPtr<class UUserWidget> ActiveWidget = nullptr;

    UPROPERTY()
    TObjectPtr<class AStaticMeshActor> OutlineActor = nullptr;

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void Hide();

public:
    ACInfoUIActor();

protected:
    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void SetTarget(AActor* InActor);
    const AActor* GetTarget() const;
    void UpdateOutlineActor(AActor* InActor);
    void HideOutlineActor();

private:
    void UpdateWidgetPosition();
};
