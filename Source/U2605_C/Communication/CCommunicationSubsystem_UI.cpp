#include "Communication/CCommunicationSubsystem_UI.h"
#include "Global.h"

void UCCommunicationSubsystem_UI::BroadcastOnShippable(int InTotalManduNum)
{
	CheckFalse(OnShippable.IsBound());
	OnShippable.Broadcast(InTotalManduNum);
}