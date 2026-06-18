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
}

FVector ACConveyorBase::GetExitTargetLocation() const
{
	FVector worldLastPointPos = SplineComp->GetLocationAtSplinePoint(SplineComp->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
	return worldLastPointPos + GetActorForwardVector() * GridConstants::HalfGridSize;
}

TArray<FIntVector> ACConveyorBase::SetupSplineFromGrid()
{
	CheckNotValidResult(SplineComp,{});
	TArray<FIntPoint> gridPoints = TArray<FIntPoint>{ FIntPoint(0, 0), FIntPoint(4, 0) };
	SplineComp->ClearSplinePoints(false);

	TArray<FIntVector> intVectors;
	FTransform actorTransform = GetActorTransform();

	for (int32 i = 0; i < gridPoints.Num(); ++i)
	{
		FVector localPointPos = FVector(gridPoints[i].X * GridConstants::HalfGridSize, gridPoints[i].Y * GridConstants::HalfGridSize, 0.0f);
		FVector worldPointPos = actorTransform.TransformPosition(localPointPos);
		SplineComp->AddSplinePoint(worldPointPos, ESplineCoordinateSpace::World, false);
		SplineComp->SetSplinePointType(i, ESplinePointType::Curve, false);

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