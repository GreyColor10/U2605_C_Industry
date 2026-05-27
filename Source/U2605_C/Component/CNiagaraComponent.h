#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "CNiagaraComponent.generated.h"

UCLASS()
class U2605_C_API UCNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
	
public:
	void SetVectorArrayUserParameter(FName InParaName, const TArray<FVector>& InArrayData);
	void SetIntArrayUserParameter(FName InParaName, const TArray<int32>& InArrayData);
};
