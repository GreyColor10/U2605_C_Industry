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

UENUM(BlueprintType)
enum class EEquipmentState : uint8
{
    Idle        UMETA(DisplayName = "대기"),
    Processing  UMETA(DisplayName = "처리중"),
    Done        UMETA(DisplayName = "완료"),
};