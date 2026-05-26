#include "MeshInstancing/CInstanedMeshSubsystem.h"
#include "Global.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/World.h"

void UCInstancedMeshSubsystem::RegisterFixedFacility(UStaticMesh* Mesh, const FTransform& Transform)
{
    UHierarchicalInstancedStaticMeshComponent* hism = GetOrCreateHISMComponent(Mesh);
    CheckNotValid(hism);

    hism->AddInstance(Transform, true);
}

void UCInstancedMeshSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    HISMSubsystemActor = InWorld.SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
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

    if (!HISMSubsystemActor->GetRootComponent())
        HISMSubsystemActor->SetRootComponent(newHISM);
    else
        newHISM->AttachToComponent(HISMSubsystemActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

    HISMContainer.Add(Mesh, newHISM);
    return newHISM;
}
