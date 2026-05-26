#include "ProductionStat/CProductionStatSubsystem.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"
#include "Communication/CCommunicationSubsystem_IO.h"

void UCProductionStatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UWorld* world = GetWorld();
	CheckNull(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_IO* commuSubsystem_IO = game->GetSubsystem<UCCommunicationSubsystem_IO>();
	CheckNotValid(commuSubsystem_IO);

	commuSubsystem_IO->GetProductDeliveredDel().AddUObject(this, &UCProductionStatSubsystem::OnProductDelivered);
}

void UCProductionStatSubsystem::Deinitialize()
{
	UWorld* world = GetWorld();
	if (IsValid(world))
	{
		if (UGameInstance* game = world->GetGameInstance())
		{
			if (UCCommunicationSubsystem_IO* ioSubsystem = game->GetSubsystem<UCCommunicationSubsystem_IO>())
				ioSubsystem->GetProductDeliveredDel().RemoveAll(this);
		}
	}
	Super::Deinitialize();
}

void UCProductionStatSubsystem::OnProductDelivered(AActor* InDeliveredEquip, const FProductData& InProductData)
{
	TotalManduNum++;

	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	commuSubsystem_UI->BroadcastOnShippable(TotalManduNum);
}