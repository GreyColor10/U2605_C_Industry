#include "Component/CNiagaraComponent.h"
#include "Global.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

void UCNiagaraComponent::SetVectorArrayUserParameter(FName InParaName, const TArray<FVector>& InArrayData)
{
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
		this,
		InParaName,
		InArrayData
	);

	MarkRenderStateDirty();
}

void UCNiagaraComponent::SetIntArrayUserParameter(FName InParaName, const TArray<int32>& InArrayData)
{
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayInt32(
		this,
		InParaName,
		InArrayData
	);

	MarkRenderStateDirty();
}

void UCNiagaraComponent::SetIntUserParameter(FName InParaName, int32 InValue)
{
	SetVariableInt(InParaName, InValue);
	MarkRenderStateDirty();
}
