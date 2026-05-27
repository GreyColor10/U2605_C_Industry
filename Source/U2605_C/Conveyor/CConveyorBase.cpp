#include "Conveyor/CConveyorBase.h"
#include "Global.h"
#include "Components/SplineComponent.h"

#include "MeshInstancing/CInstancedMeshSubsystem.h"
#include "Conveyor/CConveyorSubsystem.h"

ACConveyorBase::ACConveyorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	SetRootComponent(SplineComp);
}

void ACConveyorBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	CheckNotValid(world);

	UCInstancedMeshSubsystem* instancingSubsystem = world->GetSubsystem<UCInstancedMeshSubsystem>();
	CheckNotValid(instancingSubsystem);
	CheckNull(InstancingMesh);
	instancingSubsystem->RegisterFixedFacility(InstancingMesh, GetActorTransform());

	TArray<FIntVector> intVectorsArray;
	intVectorsArray = SetupSplineFromGrid();

	FVector targetSinkLocation = GetExitTargetLocation();

	UCConveyorSubsystem* conveyorSystem = world->GetSubsystem<UCConveyorSubsystem>();
	CheckNotValid(conveyorSystem);

	conveyorSystem->RegisterConveyor(this, SplineComp, intVectorsArray, targetSinkLocation);

	/*DrawDebugSphere(GetWorld(), GetExitTargetLocation(), 30.0f, 12, FColor::Red, true, 30.0f);*/
}

FVector ACConveyorBase::GetExitDirection() const
{
	switch (ConveyorType)
	{
	case EConveyorType::Straight:
		return GetActorForwardVector();

	case EConveyorType::Left:
		return -GetActorRightVector();

	case EConveyorType::Right:
		return GetActorRightVector();

	default:
		return GetActorForwardVector();
	}
}

FVector ACConveyorBase::GetExitTargetLocation() const
{
	TArray<FIntPoint> gridPoints = GetGridPointsForType();
	CheckFalseResult(gridPoints.Num() > 0, FVector::ZeroVector);

	const FIntPoint& lastPoint = gridPoints.Last();
	FTransform actorTransform = GetActorTransform();

	FVector localLastPointPos = FVector(lastPoint.X * GridConstants::HalfGridSize, lastPoint.Y * GridConstants::HalfGridSize, 0.0f);
	FVector worldLastPointPos = actorTransform.TransformPosition(localLastPointPos);
	return worldLastPointPos + GetExitDirection() * GridConstants::HalfGridSize;
}

TArray<FIntPoint> ACConveyorBase::GetGridPointsForType() const
{
	switch (ConveyorType)
	{
	case EConveyorType::Straight:
		return { FIntPoint(0, 0), FIntPoint(1, 0), FIntPoint(2, 0) };

	case EConveyorType::Left:
		return { FIntPoint(0, 0), FIntPoint(1, 0), FIntPoint(1, -1) };

	case EConveyorType::Right:
		return { FIntPoint(0, 0), FIntPoint(1, 0), FIntPoint(1, 1) };

	default:
		return { FIntPoint(0, 0), FIntPoint(1, 0) , FIntPoint(2, 0) };
	}
}

TArray<FIntVector> ACConveyorBase::SetupSplineFromGrid()
{
	CheckNotValidResult(SplineComp,{});
	TArray<FIntPoint> gridPoints = GetGridPointsForType();
	CheckFalseResult(gridPoints.Num() > 0, {});
	
	TArray<FIntVector> intVectors;
	SplineComp->ClearSplinePoints(false);
	FTransform actorTransform = GetActorTransform();

	for (int32 i = 0; i < gridPoints.Num(); ++i)
	{
		FVector localPointPos = FVector(gridPoints[i].X * GridConstants::HalfGridSize, gridPoints[i].Y * GridConstants::HalfGridSize, 0.0f);
		FVector worldPointPos = actorTransform.TransformPosition(localPointPos);
		SplineComp->AddSplinePoint(worldPointPos, ESplineCoordinateSpace::World, false);
		SplineComp->SetSplinePointType(i, ESplinePointType::Linear, false);
		intVectors.Add(GetGridKey(worldPointPos));
	}

	SplineComp->UpdateSpline();
	return intVectors;
}

FIntVector ACConveyorBase::GetGridKey(const FVector& InLocation)
{
	return FIntVector(
		FMath::RoundToInt(InLocation.X / GridConstants::HalfGridSize),
		FMath::RoundToInt(InLocation.Y / GridConstants::HalfGridSize),
		FMath::RoundToInt(InLocation.Z / GridConstants::HalfGridSize)
	);
}