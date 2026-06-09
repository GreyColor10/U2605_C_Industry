#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StructData/CStructDatas.h"
#include "CUserWidget_Log.generated.h"

UCLASS()
class U2605_C_API UCUserWidget_Log : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    UFUNCTION()
    void OnLogEntryAddedInternal(const FLogEntry& InLogEntry);

public:
    UFUNCTION(BlueprintImplementableEvent)
    void OnLogEntryAdded(const FLogEntry& InLogEntry);
};
