#include "Communication/CCommunicationSubsystem_UI.h"
#include "Global.h"

void UCCommunicationSubsystem_UI::BroadcastOnTotalProductCountUpdated(int InTotalProductNum)
{
	if (OnTotalProductCountUpdated.IsBound())
		OnTotalProductCountUpdated.Broadcast(InTotalProductNum);
}
