#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CSimulationTimeSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FSimulationStateChanged, bool);

UCLASS()
class U2605_C_API UCSimulationTimeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
private:
	UFUNCTION()
	void ChangeSimulationState();

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

public:
	float GetElapsedSeconds() const;

	/*FORCEINLINE const bool IsRunning() const { return bIsRunning; }*/
	FORCEINLINE FSimulationStateChanged& GetOnSimulationStateChangedDel() { return OnSimulationStateChanged; }

private:
	void StartSimulation();
	void StopSimulation();

	void BroadcastOnSimulationStateChanged(bool InIsRunning);

private:
	bool bIsRunning = false;
	float SimulationStartTime = -1.0f;
	float PausedElapsedSeconds = 0.0f;

private:
	FSimulationStateChanged OnSimulationStateChanged;
};
