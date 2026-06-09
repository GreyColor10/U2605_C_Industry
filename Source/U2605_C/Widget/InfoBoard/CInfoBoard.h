#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StructData/CStructDatas.h"
#include "CInfoBoard.generated.h"

UENUM(BlueprintType)
enum class EInfoBoardType : uint8
{
	TotalCreamBread     UMETA(DisplayName = "총 크림빵 생산량"),
	ThroughputPerMinute UMETA(DisplayName = "분당 처리량"),
	ElapsedTime         UMETA(DisplayName = "경과 시간"),
	OperatingRate       UMETA(DisplayName = "설비 가동률"),
};

UCLASS()
class U2605_C_API ACInfoBoard : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board")
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board")
	TObjectPtr<class UWidgetComponent> WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Board")
	EInfoBoardType BoardType = EInfoBoardType::TotalCreamBread;

private:
	UFUNCTION()
	void OnDashboardUpdated(const FDashboardData& InDashboardData);

public:	
	ACInfoBoard();

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
