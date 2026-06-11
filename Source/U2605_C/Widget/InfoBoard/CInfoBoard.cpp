#include "Widget/InfoBoard/CInfoBoard.h"
#include "Global.h"
#include "Components/WidgetComponent.h"

#include "Communication/CCommunicationSubsystem_UI.h"
#include "Widget/InfoBoard/CUserWidget_InfoBoard.h"

void ACInfoBoard::OnDashboardUpdated(const FDashboardData& InDashboardData)
{
	UCUserWidget_InfoBoard* widget = Cast<UCUserWidget_InfoBoard>(WidgetComponent->GetUserWidgetObject());
	CheckNull(widget);

	widget->UpdateDashboard(InDashboardData);
}

ACInfoBoard::ACInfoBoard()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(MeshComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetDrawSize(FVector2D(800.f, 400.f));
}

void ACInfoBoard::BeginPlay()
{
	Super::BeginPlay();
	
	UGameInstance* game = GetGameInstance();
	CheckNotValid(game);

	UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
	CheckNotValid(commuSubsystem_UI);

	commuSubsystem_UI->GetOnDashboardUpdatedDel().AddDynamic(
		this, &ACInfoBoard::OnDashboardUpdated);
}

void ACInfoBoard::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameInstance* game = GetGameInstance();
	if (game)
	{
		UCCommunicationSubsystem_UI* commuSubsystem_UI = game->GetSubsystem<UCCommunicationSubsystem_UI>();
		if (commuSubsystem_UI)
			commuSubsystem_UI->GetOnDashboardUpdatedDel().RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}


