#pragma once

#include "CoreMinimal.h"
#include "CFacilityType.generated.h" 

UENUM(BlueprintType)
enum class EEquipmentState : uint8
{
    Idle        UMETA(DisplayName = "대기"),
    Processing  UMETA(DisplayName = "처리중"),
    Done        UMETA(DisplayName = "완료"),
};

UENUM(BlueprintType)
enum class EProductType : uint8
{
    None        UMETA(DisplayName = "없음"),
    Flour       UMETA(DisplayName = "밀가루"),
    Dough       UMETA(DisplayName = "반죽"),
    Bread       UMETA(DisplayName = "구운빵"),
    CreamBread  UMETA(DisplayName = "크림빵"),
};