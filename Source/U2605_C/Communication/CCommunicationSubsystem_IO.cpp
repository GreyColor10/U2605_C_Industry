#include "Communication/CCommunicationSubsystem_IO.h"
#include "Global.h"

#include "Interface/IProductReceiver.h"

void UCCommunicationSubsystem_IO::BroadcastOnProductStarted(AActor* InStartedEquip, const FProductData& InProductData)
{
	CheckNotValid(InStartedEquip);

	if (OnProductStarted.IsBound())
		OnProductStarted.Broadcast(InStartedEquip, InProductData);
}

void UCCommunicationSubsystem_IO::DeliverProductTo(AActor* InTargetActor, const FProductData& InProductData)
{
	CheckNotValid(InTargetActor);

	IIProductReceiver* receiver = Cast<IIProductReceiver>(InTargetActor);
	CheckNull(receiver);

	receiver->ReceiveProduct(InProductData);
}