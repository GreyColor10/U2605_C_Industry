#include "Communication/CCommunicationSubsystem_IO.h"
#include "Global.h"

void UCCommunicationSubsystem_IO::BroadcastOnProductStarted(AActor* InStartedEquip, const FProductData& InProductData)
{
	CheckNotValid(InStartedEquip);

	if (OnProductStarted.IsBound())
		OnProductStarted.Broadcast(InStartedEquip, InProductData);
}

void UCCommunicationSubsystem_IO::BroadcastOnProductDelivered(AActor* InDeliveredEquip, const FProductData& InProductData)
{
	CheckNotValid(InDeliveredEquip);

	if(OnProductDelivered.IsBound())
		OnProductDelivered.Broadcast(InDeliveredEquip, InProductData);
}
