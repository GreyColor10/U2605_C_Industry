#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Conveyor/FConveyorSimulator.h"
#include "CConveyorSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FNiagaraCompActive, const bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FNiagaraCompSetParameter, FName , const TArray<FVector>&);

UCLASS()
class U2605_C_API UCConveyorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TObjectPtr<class UCConveyorGraph> Graph;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	void RegisterConveyor(AActor* InActor, class USplineComponent* InSpline, TArray<FIntVector>& InGridPoints);
	void BuildConveyorNetwork();

	// Storage가 자기 등록 시 호출
	void RegisterSink(AActor* InSink);
	void UnregisterSink(AActor* InSink);

private:
	void StartSimulationIfNeeded();
	void UpdateProductItemsFlow();
	void OnBroadCast(TArray<FProductArrival>& InArrived);
	
private:
	void OnProductStarted(AActor* InSourceStorage, const FProductData& InProductData);

public:
	FORCEINLINE FNiagaraCompActive& GetOnNiagaraCompActive() { return OnNiagaraCompActive; };
	FORCEINLINE FNiagaraCompSetParameter& GetOnNiagaraCompSetParameter() { return OnNiagaraCompSetParameter; };

private:
	TUniquePtr<class FConveyorSimulator> Simulator;

private:
	FTimerHandle ConveyorHandle;

	FNiagaraCompActive OnNiagaraCompActive;
	FNiagaraCompSetParameter OnNiagaraCompSetParameter;
};
