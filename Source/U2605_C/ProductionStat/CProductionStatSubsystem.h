#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CProductionStatSubsystem.generated.h"

UCLASS()
class U2605_C_API UCProductionStatSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void ReceiveFinalProduct();;

private:
	int TotalProductNum = 0;
};
