#include "Communication/CCommunicationSubsystem_UI.h"
#include "Global.h"

void UCCommunicationSubsystem_UI::BroadcastOnTotalProductCountUpdated(int InTotalProductNum)
{
	if (OnTotalProductCountUpdated.IsBound())
		OnTotalProductCountUpdated.Broadcast(InTotalProductNum);
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

void UCCommunicationSubsystem_UI::BroadcastOnUITargetChanged(AActor* InTarget)
{
	OnUITargetChanged.ExecuteIfBound(InTarget);
}

const AActor* UCCommunicationSubsystem_UI::BroadcastOnOnUITargetGotten()
{
	if (OnUITargetGotten.IsBound())
		return OnUITargetGotten.Execute();

	return nullptr;
}