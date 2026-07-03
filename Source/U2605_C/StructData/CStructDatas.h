#pragma once

#include "CoreMinimal.h"
#include "Types/CFacilityType.h"
#include "CStructDatas.generated.h" 

USTRUCT()
struct FProductData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	EProductType ProductType = EProductType::None;

	UPROPERTY()
	float CurrentDistance = 0.0f;

	UPROPERTY()
	bool bArrived = false;

	UPROPERTY()
	int32 ProcessStage = 0;

	UPROPERTY()
	bool  bBlocked = false;
};

USTRUCT(BlueprintType)
struct FStorageInfoData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EProductType ProductType = EProductType::None;

	UPROPERTY(BlueprintReadOnly)
	int32 StoredCount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxCapacity = 0;
};

USTRUCT(BlueprintType)
struct FProcessorInfoData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EEquipmentState State = EEquipmentState::Idle;

	UPROPERTY(BlueprintReadOnly)
	float ProcessingTime = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float Progress = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	TMap<EProductType, int32> RequiredProducts;

	UPROPERTY(BlueprintReadOnly)
	TMap<EProductType, int32> ArrivedCount;

	UPROPERTY(BlueprintReadOnly)
	EProductType ProducedProduct = EProductType::None;
};

USTRUCT(BlueprintType)
struct FDashboardData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 TotalCreamBread = 0;

	UPROPERTY(BlueprintReadOnly)
	float ThroughputPerMinute = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float ElapsedSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float OperatingRate = 0.0f;
};

UENUM(BlueprintType)
enum class ELogEventType : uint8
{
	Info    UMETA(DisplayName = "일반"),
	Warning UMETA(DisplayName = "경고"),
	Alert   UMETA(DisplayName = "알림"),
};

USTRUCT(BlueprintType)
struct FLogEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ELogEventType EventType = ELogEventType::Info;

	UPROPERTY(BlueprintReadOnly)
	FString Message = TEXT("");

	UPROPERTY(BlueprintReadOnly)
	FString TimestampText = TEXT("");

	static FString FormatTimestamp()
	{
		FDateTime now = FDateTime::Now();
		return FString::Printf(TEXT("[%02d:%02d:%02d]"),
			now.GetHour(), now.GetMinute(), now.GetSecond());
	}
};

USTRUCT(BlueprintType)
struct FScenarioComparisonResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float NormalProduction = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float ScenarioProduction = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float ProductionChangePercent = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float NormalThroughput = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float ScenarioThroughput = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float DurationSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsValid = false;
};

struct FEquipmentOperatingRecord
{
	float AccumulatedSeconds = 0.0f;
	float CurrentStartTime = -1.0f;
};