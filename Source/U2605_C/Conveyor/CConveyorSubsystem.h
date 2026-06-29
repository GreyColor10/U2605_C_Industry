#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Conveyor/FConveyorSimulator.h"
#include "CConveyorSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FNiagaraCompActive, const bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FNiagaraCompSetParticlePosition, FName , const TArray<FVector>&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FNiagaraCompSetMeshIndices, FName, const TArray<int32>&);

UCLASS()
class U2605_C_API UCConveyorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TObjectPtr<class UCConveyorGraph> Graph;

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void PostInitialize() override;
	void Deinitialize() override;

public:
	void RegisterConveyor(AActor* InActor, class USplineComponent* InSpline, const TArray<FIntVector>& InGridPoints, const FVector InSinkPosition);
	void BuildConveyorNetwork();

	void RegisterSink(AActor* InSink);
	void UnregisterSink(AActor* InSink);

	void Pause();
	void Resume();

private:
	void StartSimulationIfNeeded();
	void UpdateProductItemsFlow();
	void DeliverArrivedProducts(TArray<FProductArrival>& InArrived);

	const bool IsShipBlockedFrom(const FVector& InSourceLocation) const;

private:
	void OnProductStarted(AActor* InSourceStorage, const FProductData& InProductData);

	void OnSimulationStateChanged(bool InIsRunning);

public:
	FORCEINLINE FNiagaraCompActive& GetOnNiagaraCompActive() { return OnNiagaraCompActive; };
	FORCEINLINE FNiagaraCompSetParticlePosition& GetOnNiagaraCompSetParticlePosition() { return OnNiagaraCompSetParticlePosition; };
	FORCEINLINE FNiagaraCompSetMeshIndices& GetOnNiagaraCompSetMeshIndices() { return OnNiagaraCompSetMeshIndices; }

	FORCEINLINE bool IsPaused() const { return bIsPaused; }

private:
	TUniquePtr<FConveyorSimulator> Simulator;

private:
	FTimerHandle ConveyorHandle;

	FNiagaraCompActive OnNiagaraCompActive;
	FNiagaraCompSetParticlePosition OnNiagaraCompSetParticlePosition;
	FNiagaraCompSetMeshIndices OnNiagaraCompSetMeshIndices;

	bool bIsPaused = false;
};
