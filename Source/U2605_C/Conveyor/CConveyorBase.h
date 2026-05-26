#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/CFacilityType.h"
#include "CConveyorBase.generated.h"

UCLASS(Abstract)
class U2605_C_API ACConveyorBase : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline")
	TObjectPtr<class USplineComponent> SplineComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMesh> InstancingMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor|Data")
	TArray<FIntPoint> GridPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor|Data")
	float GridSize = 50.0f;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Type", meta = (DeprecatedProperty, DeprecationMessage = "삭제 예정인 데이터입니다."))
	EConveyorType ConveyorType = EConveyorType::Max;*/

public:	
	ACConveyorBase();

protected:
	void BeginPlay() override;

public:
	FORCEINLINE USplineComponent* GetSplineComponent() const { return SplineComp; }
	/*FORCEINLINE const TArray<FIntPoint>& GetGridPoints() const { return GridPoints; }*/

private:
	TArray<FIntVector> SetupSplineFromGrid();
	FIntVector GetGridKey(const FVector& InLocation);
};
