#include "Component/ActorComponent/CScreenWidgetComponent.h"
#include "Global.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include "Widget/CUserWidget_Screen.h"
#include "Communication/CCommunicationSubsystem_UI.h"

void UCScreenWidgetComponent::OnStoredFinalProductUpdated(int InStoredFinalProductNum)
{
	UI_Player->OnStoredFinalProductUpdated(InStoredFinalProductNum);
}

UCScreenWidgetComponent::UCScreenWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCScreenWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CheckNull(UI_ScreenClass);

	APawn* pawn = Cast<APawn>(GetOwner());
	CheckNotValid(pawn);

	APlayerController* playerCon = Cast<APlayerController>(pawn->GetController());
	CheckNotValid(playerCon);

	UI_Player = CreateWidget<UCUserWidget_Screen>(playerCon, UI_ScreenClass);
	CheckNotValid(UI_Player);

	UI_Player->AddToViewport();

	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	commuSubsystem_UI->GetOnStoredFinalProductUpdatedDel().AddDynamic(this, &UCScreenWidgetComponent::OnStoredFinalProductUpdated);
}

void UCScreenWidgetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(UI_Player))
	{
		UI_Player->RemoveFromParent();
		UI_Player = nullptr;
	}

	UWorld* world = GetWorld();
	CheckNotValid(world);

	UGameInstance* game = world->GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	commuSubsystem_UI->GetOnStoredFinalProductUpdatedDel().RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}
