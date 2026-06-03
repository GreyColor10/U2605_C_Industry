#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StructData/CStructDatas.h"
#include "CUserWidget_Info_Storage.generated.h"

UCLASS()
class U2605_C_API UCUserWidget_Info_Storage : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnStorageInfoUpdatedInternal(const FStorageInfoData& InStorageInfoData);

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnStorageInfoUpdated(const FStorageInfoData& InStorageInfoData);
};
