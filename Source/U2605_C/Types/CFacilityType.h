#pragma once

#include "CoreMinimal.h"
#include "CFacilityType.generated.h" 

UENUM(BlueprintType)
enum class EConveyorType : uint8
{
	Straight, 
	Corner, 
	Max UMETA(DisplayName = "정의되지 않음 (에러 방지용)")
};