#pragma once

#include "CoreMinimal.h"
#include "CFacilityType.generated.h" 

UENUM(BlueprintType)
enum class EConveyorType : uint8
{
    Straight    UMETA(DisplayName = "직진"),
    Left        UMETA(DisplayName = "좌회전"),
    Right       UMETA(DisplayName = "우회전"),
};