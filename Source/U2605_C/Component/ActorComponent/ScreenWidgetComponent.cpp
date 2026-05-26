#include "Component/ActorComponent/ScreenWidgetComponent.h"
#include "Global.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include "Widget/CUserWidget_Screen.h"
#include "Communication/CommunicationSubsystem_UI.h"

void UScreenWidgetComponent::OnShippable(FProductData InProductData)
{
	UI_Player->OnProductShipAble(InProductData);
}

UScreenWidgetComponent::UScreenWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UScreenWidgetComponent::BeginPlay()
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

	UCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	commuSubsystem_UI->GetShippableDel().AddDynamic(this, &UScreenWidgetComponent::OnShippable);
}

void UScreenWidgetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

	UCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	commuSubsystem_UI->GetShippableDel().RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}
