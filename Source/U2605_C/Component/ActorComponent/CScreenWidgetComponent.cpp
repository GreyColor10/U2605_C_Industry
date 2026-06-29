#include "Component/ActorComponent/CScreenWidgetComponent.h"
#include "Global.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include "Widget/CUserWidget_Screen.h"
#include "SimulationTime/CSimulationTimeSubsystem.h"

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

	UCSimulationTimeSubsystem* timeSubsystem= world->GetSubsystem<UCSimulationTimeSubsystem>();
	CheckNotValid(timeSubsystem);

	timeSubsystem->GetOnSimulationStateChangedDel().AddUObject(this, &UCScreenWidgetComponent::OnSimulationStateChanged);
}

void UCScreenWidgetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(UI_Player))
	{
		UI_Player->RemoveFromParent();
		UI_Player = nullptr;
	}

	UWorld* world = GetWorld();
	if (IsValid(world))
	{
		UCSimulationTimeSubsystem* timeSubsystem = world->GetSubsystem<UCSimulationTimeSubsystem>();
		if(IsValid(timeSubsystem))
			timeSubsystem->GetOnSimulationStateChangedDel().RemoveAll(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void UCScreenWidgetComponent::OnSimulationStateChanged(bool InIsRunning)
{
	UI_Player->OnSimulationStateChanged(InIsRunning);
}