#include "Communication/CCommunicationSubsystem_UI.h"
#include "Global.h"

void UCCommunicationSubsystem_UI::BroadcastOnExported()
{
	if (OnExported.IsBound())
		OnExported.Broadcast();
}

void UCCommunicationSubsystem_UI::BroadcastOnSimulationStateUIChanged()
{
	if (OnSimulationStateUIChanged.IsBound())
		OnSimulationStateUIChanged.Broadcast();
}

void UCCommunicationSubsystem_UI::BroadcastOnShortageScenarioStarted(const float InShortageDuration)
{
	if (OnShortageScenarioStarted.IsBound())
		OnShortageScenarioStarted.Broadcast(InShortageDuration);
}

void UCCommunicationSubsystem_UI::BroadcastOnStorageInfoUpdated(const FStorageInfoData& InStorageInfoData)
{
	if (OnStorageInfoUpdated.IsBound())
		OnStorageInfoUpdated.Broadcast(InStorageInfoData);
}

void UCCommunicationSubsystem_UI::BroadcastOnProcessorInfoUpdated(const FProcessorInfoData& InProcessorInfoData)
{
	if (OnProcessorInfoUpdated.IsBound())
		OnProcessorInfoUpdated.Broadcast(InProcessorInfoData);
}

void UCCommunicationSubsystem_UI::BroadcastOnDashboardUpdated(const FDashboardData& InData)
{
	if (OnDashboardUpdated.IsBound())
		OnDashboardUpdated.Broadcast(InData);
}

void UCCommunicationSubsystem_UI::BroadcastOnLogEntryAdded(const FLogEntry& InEntry)
{
	if (OnLogEntryAdded.IsBound())
		OnLogEntryAdded.Broadcast(InEntry);
}

void UCCommunicationSubsystem_UI::BroadcastOnProcessorProgressUpdated(float InProgress)
{
	if (OnProcessorProgressUpdated.IsBound())
		OnProcessorProgressUpdated.Broadcast(InProgress);
}

void UCCommunicationSubsystem_UI::BroadcastOnScenarioComparisonReady(const FScenarioComparisonResult& InResult)
{
	if (OnScenarioComparisonReady.IsBound())
		OnScenarioComparisonReady.Broadcast(InResult);
}

void UCCommunicationSubsystem_UI::BroadcastOnScenarioRemainingUpdated(float InRemainingSeconds)
{
	if (OnScenarioRemainingUpdated.IsBound())
		OnScenarioRemainingUpdated.Broadcast(InRemainingSeconds);
}

void UCCommunicationSubsystem_UI::BroadcastOnScenarioDeactived()
{
	if (OnScenarioDeactived.IsBound())
		OnScenarioDeactived.Broadcast();
}

void UCCommunicationSubsystem_UI::BroadcastOnUITargetChanged(AActor* InTarget)
{
	OnUITargetChanged.ExecuteIfBound(InTarget);
}

void UCCommunicationSubsystem_UI::BroadcastOnProcessingTimeChangeRequested(UClass* InProcessorClass, float InProcessingTime)
{
	if (OnProcessingTimeChangeRequested.IsBound())
		OnProcessingTimeChangeRequested.Broadcast(InProcessorClass, InProcessingTime);
}

void UCCommunicationSubsystem_UI::BroadcastOnProcessingTimeChangeEnded()
{
	if (OnProcessingTimeChangeEnded.IsBound())
		OnProcessingTimeChangeEnded.Broadcast();
}

const AActor* UCCommunicationSubsystem_UI::GetCurrentUITarget()
{
	if (OnUITargetGotten.IsBound())
		return OnUITargetGotten.Execute();

	return nullptr;
}