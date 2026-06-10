#include "Communication/CCommunicationSubsystem_UI.h"
#include "Global.h"

void UCCommunicationSubsystem_UI::StartFactory()
{
	BroadcastOnSimulationStateChanged(true);
}

void UCCommunicationSubsystem_UI::StopFactory()
{
	BroadcastOnSimulationStateChanged(false);
}

void UCCommunicationSubsystem_UI::BroadcastOnStoredFinalProductUpdated(int InStoredFinalProductNum)
{
	if (OnStoredFinalProductUpdated.IsBound())
		OnStoredFinalProductUpdated.Broadcast(InStoredFinalProductNum);
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

void UCCommunicationSubsystem_UI::BroadcastOnProcessingTimeChangeRequested(UClass* InProcessorClass, float InProcessingTime)
{
	if (OnProcessingTimeChangeRequested.IsBound())
		OnProcessingTimeChangeRequested.Broadcast(InProcessorClass, InProcessingTime);
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

void UCCommunicationSubsystem_UI::BroadcastOnSimulationStateChanged(bool InIsRunning)
{
	if (OnSimulationStateChanged.IsBound())
		OnSimulationStateChanged.Broadcast(InIsRunning);
}

void UCCommunicationSubsystem_UI::BroadcastOnUITargetChanged(AActor* InTarget)
{
	OnUITargetChanged.ExecuteIfBound(InTarget);
}

const AActor* UCCommunicationSubsystem_UI::GetCurrentUITarget()
{
	if (OnUITargetGotten.IsBound())
		return OnUITargetGotten.Execute();

	return nullptr;
}
