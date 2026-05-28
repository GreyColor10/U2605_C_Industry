#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CInstancedMeshSubsystem.generated.h"

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
    int32 RegisterFixedFacility(UStaticMesh* Mesh, const FTransform& Transform);

public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    void SetCustomData(UStaticMesh* Mesh, int32 InstanceIndex, int32 DataIndex, float Value);

private:
    UHierarchicalInstancedStaticMeshComponent* GetOrCreateHISMComponent(UStaticMesh* Mesh);
};
