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

	UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(uiSubsystem);

	FLogEntry entry;
	entry.EventType = InEventType;
	entry.Message = InLogMessage;
	entry.TimestampText = FLogEntry::FormatTimestamp();

	uiSubsystem->BroadcastOnLogEntryAdded(entry);
}