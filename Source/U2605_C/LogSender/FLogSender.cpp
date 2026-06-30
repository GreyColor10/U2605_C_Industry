#include "LogSender/FLogSender.h"
#include "Global.h"

#include "Communication/CCommunicationSubsystem_UI.h"

void FLogSender::SendLogMessage(UWorld* InWorld, ELogEventType InEventType, FString InLogMessage)
{
	CheckNotValid(InWorld);

	UGameInstance* game = InWorld->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(uiSubsystem);

	FLogEntry entry;
	entry.EventType = InEventType;
	entry.Message = InLogMessage;
	entry.TimestampText = FLogEntry::FormatTimestamp();

	uiSubsystem->BroadcastOnLogEntryAdded(entry);
}
