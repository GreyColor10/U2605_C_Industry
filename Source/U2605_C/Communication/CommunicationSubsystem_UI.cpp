#include "Communication/CommunicationSubsystem_UI.h"
#include "Global.h"

void UCommunicationSubsystem_UI::BroadcastOnShippable(FProductData& InProductData)
{
	CheckFalse(OnShippable.IsBound());
	OnShippable.Broadcast(InProductData);
}