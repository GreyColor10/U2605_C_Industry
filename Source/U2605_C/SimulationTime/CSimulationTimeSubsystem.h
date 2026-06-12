#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CSimulationTimeSubsystem.generated.h"

UCLASS()
class U2605_C_API UCSimulationTimeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
private:
	UFUNCTION(BlueprintCallable, Category = "ProductionStat")
	void ChangeSimulationState();

public:
	float GetElapsedSeconds() const;

	/*FORCEINLINE const bool IsRunning() const { return bIsRunning; }*/

private:
	void StartSimulation();
	void StopSimulation();

private:
	bool bIsRunning = false;
	float SimulationStartTime = -1.0f;
	float PausedElapsedSeconds = 0.0f;
};
