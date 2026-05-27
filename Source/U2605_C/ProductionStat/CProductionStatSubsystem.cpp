#include "ProductionStat/CProductionStatSubsystem.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"

void UCProductionStatSubsystem::ReceiveFinalProduct()
{
	TotalProductNum++;

	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	commuSubsystem_UI->BroadcastOnTotalProductCountUpdated(TotalProductNum);
}