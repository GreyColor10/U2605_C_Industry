#include "Component/ActorComponent/CLogComponent.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"

UCLogComponent::UCLogComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCLogComponent::SendLogMessage(ELogEventType InEventType, FString InLogMessage)
{
	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	FLogEntry entry;
	entry.EventType = InEventType;
	entry.Message = InLogMessage;
	entry.TimestampText = FLogEntry::FormatTimestamp();

	commuSubsystem_UI->BroadcastOnLogEntryAdded(entry);
}