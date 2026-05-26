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

	UCConveyorSubsystem* conveyorSystem = world->GetSubsystem<UCConveyorSubsystem>();
	CheckNotValid(conveyorSystem);
	CheckFalse(GridPoints.Num() > 0);

	conveyorSystem->RegisterConveyor(this, SplineComp, intVectorsArray);
}

TArray<FIntVector> ACConveyorBase::SetupSplineFromGrid()
{
	CheckNotValidResult(SplineComp,{});
	CheckFalseResult(GridPoints.Num(),{});
	
	TArray<FIntVector> intVectors;
	SplineComp->ClearSplinePoints(false);
	FTransform actorTransform = GetActorTransform();

	for (int32 i = 0; i < GridPoints.Num(); ++i)
	{
		FVector localPointPos = FVector(GridPoints[i].X * GridSize, GridPoints[i].Y * GridSize, 0.0f);
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
		FMath::RoundToInt(InLocation.X / GridSize),
		FMath::RoundToInt(InLocation.Y / GridSize),
		FMath::RoundToInt(InLocation.Z / GridSize)
	);
}

