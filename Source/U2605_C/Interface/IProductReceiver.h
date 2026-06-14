#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StructData/CStructDatas.h" 
#include "IProductReceiver.generated.h"

UINTERFACE(MinimalAPI)
class UIProductReceiver : public UInterface
{
	GENERATED_BODY()
};

class U2605_C_API IIProductReceiver
{
	GENERATED_BODY()

public:
	virtual bool ReceiveProduct(const FProductData& InProductData) = 0;
};
