#include "MeshInstancing/CInstancedMeshSubsystem.h"
#include "Global.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/World.h"

int32 UCInstancedMeshSubsystem::RegisterFixedFacility(UStaticMesh* Mesh, const FTransform& Transform)
{
    UHierarchicalInstancedStaticMeshComponent* hism = GetOrCreateHISMComponent(Mesh);
    CheckNotValidResult(hism, 0);

    return hism->AddInstance(Transform, true);
}

void UCInstancedMeshSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    HISMSubsystemActor = InWorld.SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
}

void UCInstancedMeshSubsystem::SetCustomData(UStaticMesh* Mesh, int32 InstanceIndex, int32 DataIndex, float Value)
{
    UHierarchicalInstancedStaticMeshComponent* hism = GetOrCreateHISMComponent(Mesh);
    CheckNotValid(hism);

    hism->SetCustomDataValue(InstanceIndex, DataIndex, Value, true);
}

UHierarchicalInstancedStaticMeshComponent* UCInstancedMeshSubsystem::GetOrCreateHISMComponent(UStaticMesh* Mesh)
{
    if (!Mesh || !HISMSubsystemActor) return nullptr;

    if (HISMContainer.Contains(Mesh))
        return HISMContainer[Mesh];

    UHierarchicalInstancedStaticMeshComponent* newHISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(HISMSubsystemActor);
    newHISM->RegisterComponent();
    newHISM->SetStaticMesh(Mesh);

    newHISM->SetMobility(EComponentMobility::Static);
    newHISM->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    newHISM->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    newHISM->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    newHISM->NumCustomDataFloats = 1;

    if (!HISMSubsystemActor->GetRootComponent())
        HISMSubsystemActor->SetRootComponent(newHISM);
    else
        newHISM->AttachToComponent(HISMSubsystemActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

    HISMContainer.Add(Mesh, newHISM);
    return newHISM;
}
