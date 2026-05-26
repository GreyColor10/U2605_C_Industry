#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IClickable.generated.h"

UINTERFACE(MinimalAPI)
class UIClickable : public UInterface
{
	GENERATED_BODY()
};

class U2605_C_API IIClickable
{
	GENERATED_BODY()

public:
	// 추후 추가 예정:
// virtual void OnClicked(const FHitResult& InHit) = 0;
// virtual class UUserWidget* GetInfoWidget() const = 0;
};
