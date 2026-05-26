#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CInstanedMeshSubsystem.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class U2605_C_API UCInstancedMeshSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
    UPROPERTY()
    TMap<UStaticMesh*, UHierarchicalInstancedStaticMeshComponent*> HISMContainer;

    UPROPERTY()
    TObjectPtr<AActor> HISMSubsystemActor;

public:
    UFUNCTION(BlueprintCallable, Category = "Facility System")
    void RegisterFixedFacility(UStaticMesh* Mesh, const FTransform& Transform);

public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
    UHierarchicalInstancedStaticMeshComponent* GetOrCreateHISMComponent(UStaticMesh* Mesh);
};
