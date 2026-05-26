#include "ProductionEquipment/CProductionEquipment_Base.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"

#include "MeshInstancing/CInstancedMeshSubsystem.h"

ACProductionEquipment_Base::ACProductionEquipment_Base()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);
}

void ACProductionEquipment_Base::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* world = GetWorld();
	CheckNotValid(world);

	UCInstancedMeshSubsystem* instancingSubsystem = world->GetSubsystem<UCInstancedMeshSubsystem>();
	CheckNotValid(instancingSubsystem);
	CheckNull(InstancingMesh);
	instancingSubsystem->RegisterFixedFacility(InstancingMesh, GetActorTransform());
}
