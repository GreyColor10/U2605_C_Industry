#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CScenarioSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FShortageScenarioActiveChanged, bool);

UCLASS()
class U2605_C_API UCScenarioSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
private:
	UFUNCTION()
	void OnShortageScenarioStarted(const float InDuration);

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void PostInitialize() override;
	void Deinitialize() override;

private:
	void StartMeasurement(const float InDuration);
	void EndMeasurement();

	void StartScenarioRemainingTimer();
	void StopScenarioRemainingTimer();
	void ScenarioRemainingUpdated();

	float GetScenarioRemainingSeconds() const;

	void ResumeScenario();
	void PauseScenario();

private:
	void OnSimulationStateChanged(bool InIsRunning);

public:
	FORCEINLINE FShortageScenarioActiveChanged& GetOnShortageScenarioActiveChanged() { return OnShortageScenarioActiveChanged; }
	
private:
	TPimplPtr<class FLogSender> LogSender;

private:
	FTimerHandle ScenarioHandle;
	FTimerHandle ScenarioRemainingHandle;

private:
	FShortageScenarioActiveChanged OnShortageScenarioActiveChanged;

private:
	bool bIsMeasuring = false;
	float ScenarioStartTime = -1.0f;
	float ScenarioDuration = -1.0f;
};
