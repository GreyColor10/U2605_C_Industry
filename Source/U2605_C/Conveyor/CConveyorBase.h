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

public:	
	ACConveyorBase();

protected:
	void BeginPlay() override;

public:
	FORCEINLINE USplineComponent* GetSplineComponent() const { return SplineComp; }

private:
	FVector GetExitTargetLocation() const;
	TArray<FIntVector> SetupSplineFromGrid();

private:
	FIntVector GetGridKey(const FVector& InLocation);
};
