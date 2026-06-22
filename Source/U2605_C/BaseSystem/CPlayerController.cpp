#include "BaseSystem/CPlayerController.h"
#include "Global.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Interface/IClickable.h"
#include "Communication/CCommunicationSubsystem_UI.h"

void ACPlayerController::BeginPlay()
{
	Super::BeginPlay();
	bShowMouseCursor = true;
	FInputModeGameAndUI inputMode;
	inputMode.SetHideCursorDuringCapture(false);
	SetInputMode(inputMode);

	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ACPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	CheckNotValid(EnhancedInputComponent);

	EnhancedInputComponent->BindAction(MouseClickAction, ETriggerEvent::Triggered, this, &ACPlayerController::OnMouseClick);
}

void ACPlayerController::OnMouseClick(const FInputActionValue& Value)
{
	UWorld* world = GetWorld();
	CheckNotValid(world);

	FHitResult HitResult;
	bool bHit = GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ClickAbleChannel), true, HitResult);

	if (bHit && HitResult.GetActor())
	{
		AActor* hitActor = HitResult.GetActor();

		IIClickable* clickable = Cast<IIClickable>(hitActor);
		CheckNull(clickable);

		UGameInstance* game = world->GetGameInstance();
		CheckNotValid(game);

		UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
		CheckNotValid(uiSubsystem);

		uiSubsystem->BroadcastOnUITargetChanged(hitActor);

		//초기화 순서 문제가 있어, 나중에 호출함
		clickable->OnClicked(HitResult);
	}
}
