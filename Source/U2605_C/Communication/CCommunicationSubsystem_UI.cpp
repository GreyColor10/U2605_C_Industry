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
