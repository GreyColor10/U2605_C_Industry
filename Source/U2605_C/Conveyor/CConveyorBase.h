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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Type")
	EConveyorType ConveyorType = EConveyorType::Straight;

public:	
	ACConveyorBase();

protected:
	void BeginPlay() override;

public:
	FORCEINLINE USplineComponent* GetSplineComponent() const { return SplineComp; }
	FORCEINLINE EConveyorType GetConveyorType() const { return ConveyorType; }

public:
	FVector GetExitDirection() const;
	FVector GetExitTargetLocation() const;

private:
	TArray<FIntPoint> GetGridPointsForType() const;
	TArray<FIntVector> SetupSplineFromGrid();
	FIntVector GetGridKey(const FVector& InLocation);

private:
	static constexpr float HalfGridSize = 50.0f;
};
