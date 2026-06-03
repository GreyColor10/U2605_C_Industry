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
	virtual void OnClicked(const FHitResult& InHit) = 0;
};
