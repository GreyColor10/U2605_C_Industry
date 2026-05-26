#include "Communication/CommunicationSubsystem_IO.h"
#include "Global.h"

void UCommunicationSubsystem_IO::BroadcastOnProductStarted(AActor* InStartedEquip, const FProductData& InProductData)
{
	CheckFalse(OnProductStarted.IsBound());
	CheckNotValid(InStartedEquip);

	OnProductStarted.Broadcast(InStartedEquip, InProductData);
}

void UCommunicationSubsystem_IO::BroadcastOnProductDelivered(AActor* InDeliveredEquip, const FProductData& InProductData)
{
	CheckFalse(OnProductDelivered.IsBound());
	CheckNotValid(InDeliveredEquip);

	OnProductDelivered.Broadcast(InDeliveredEquip, InProductData);
}
