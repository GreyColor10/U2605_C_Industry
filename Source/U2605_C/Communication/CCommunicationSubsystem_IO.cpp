#include "Communication/CCommunicationSubsystem_IO.h"
#include "Global.h"

void UCCommunicationSubsystem_IO::BroadcastOnProductStarted(AActor* InStartedEquip, const FProductData& InProductData)
{
	CheckFalse(OnProductStarted.IsBound());
	CheckNotValid(InStartedEquip);

	OnProductStarted.Broadcast(InStartedEquip, InProductData);
}

void UCCommunicationSubsystem_IO::BroadcastOnProductDelivered(AActor* InDeliveredEquip, const FProductData& InProductData)
{
	CheckFalse(OnProductDelivered.IsBound());
	CheckNotValid(InDeliveredEquip);

	OnProductDelivered.Broadcast(InDeliveredEquip, InProductData);
}
