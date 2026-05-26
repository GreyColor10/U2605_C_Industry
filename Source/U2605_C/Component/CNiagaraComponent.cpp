#include "Component/CNiagaraComponent.h"
#include "Global.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

void UCNiagaraComponent::SetVectorArrayUserParameter(FName InParaName, const TArray<FVector>& InArrayData)
{
	ResetSystem();

	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
		this,
		InParaName,
		InArrayData
	);

	MarkRenderStateDirty();
}