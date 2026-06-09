#include "Widget/InfoBoard/CInfoBoard.h"
#include "Global.h"
#include "Components/WidgetComponent.h"

#include "Communication/CCommunicationSubsystem_UI.h"
#include "Widget/InfoBoard/CUserWidget_InfoBoard.h"

void ACInfoBoard::OnDashboardUpdated(const FDashboardData& InDashboardData)
{
	UCUserWidget_InfoBoard* widget = Cast<UCUserWidget_InfoBoard>(WidgetComponent->GetUserWidgetObject());
	CheckNull(widget);

	switch (BoardType)
	{
	case EInfoBoardType::TotalCreamBread:
		widget->UpdateValue(FString::FromInt(InDashboardData.TotalCreamBread));
		break;

	case EInfoBoardType::ThroughputPerMinute:
		widget->UpdateValue(FString::Printf(TEXT("%.1f /min"), InDashboardData.ThroughputPerMinute));
		break;

	case EInfoBoardType::ElapsedTime:
	{
		int32 minutes = (int32)InDashboardData.ElapsedSeconds / 60;
		int32 seconds = (int32)InDashboardData.ElapsedSeconds % 60;
		widget->UpdateValue(FString::Printf(TEXT("%02d:%02d"), minutes, seconds));
		break;
	}

	case EInfoBoardType::OperatingRate:
		widget->UpdateValue(FString::Printf(TEXT("%.0f%%"), InDashboardData.OperatingRate * 100.f));
		break;
	}
}

ACInfoBoard::ACInfoBoard()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(MeshComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetDrawSize(FVector2D(400.f, 200.f));

	WidgetComponent->SetRelativeLocation(FVector(1.f, 0.f, 0.f));
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


