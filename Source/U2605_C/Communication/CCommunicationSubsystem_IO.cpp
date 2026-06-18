#include "Communication/CCommunicationSubsystem_IO.h"
#include "Global.h"

#include "Interface/IProductReceiver.h"

void UCCommunicationSubsystem_IO::BroadcastOnProductStarted(AActor* InStartedEquip, const FProductData& InProductData)
{
	CheckNotValid(InStartedEquip);

	if (OnProductStarted.IsBound())
		OnProductStarted.Broadcast(InStartedEquip, InProductData);
}

bool UCCommunicationSubsystem_IO::DeliverProductTo(AActor* InTargetActor, const FProductData& InProductData)
{
	CheckNotValidResult(InTargetActor, false);

	IIProductReceiver* receiver = Cast<IIProductReceiver>(InTargetActor);
	CheckNullResult(receiver, false);

	return receiver->ReceiveProduct(InProductData);
}