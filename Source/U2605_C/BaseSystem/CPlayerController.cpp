#include "BaseSystem/CPlayerController.h"
#include "Global.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CineCameraActor.h"

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

	DefaultViewTarget = GetViewTarget();

	UWorld* world = GetWorld();
	CheckNotValid(world);

	for (const FName& tag : CineCameraTags)
	{
		TArray<AActor*> foundActors;
		UGameplayStatics::GetAllActorsOfClassWithTag(world, ACineCameraActor::StaticClass(), tag, foundActors);

		if (foundActors.Num() > 0) CineCameras.Add(Cast<ACineCameraActor>(foundActors[0]));
		else CineCameras.Add(nullptr);
	}
}

void ACPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	CheckNotValid(EnhancedInputComponent);

	EnhancedInputComponent->BindAction(MouseClickAction, ETriggerEvent::Triggered, this, &ACPlayerController::OnMouseClick);

	EnhancedInputComponent->BindAction(Camera0Action, ETriggerEvent::Triggered, this, &ACPlayerController::OnCamera0);
	EnhancedInputComponent->BindAction(Camera1Action, ETriggerEvent::Triggered, this, &ACPlayerController::OnCamera1);
	EnhancedInputComponent->BindAction(Camera2Action, ETriggerEvent::Triggered, this, &ACPlayerController::OnCamera2);
	EnhancedInputComponent->BindAction(Camera3Action, ETriggerEvent::Triggered, this, &ACPlayerController::OnCamera3);
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

void ACPlayerController::OnCamera0(const FInputActionValue& Value)
{
	SwitchToCamera(DefaultViewTarget.Get());
}

void ACPlayerController::OnCamera1(const FInputActionValue& Value)
{
	SwitchToCameraByIndex(0);
}

void ACPlayerController::OnCamera2(const FInputActionValue& Value)
{
	SwitchToCameraByIndex(1);
}

void ACPlayerController::OnCamera3(const FInputActionValue& Value)
{
	SwitchToCameraByIndex(2);
}

void ACPlayerController::SwitchToCameraByIndex(int32 Index)
{
	CheckFalse(CineCameras.IsValidIndex(Index));
	
	SwitchToCamera(CineCameras[Index]);
}

void ACPlayerController::SwitchToCamera(AActor* Target)
{
	CheckNotValid(Target);
	
	SetViewTargetWithBlend(Target, ViewBlendTime);
}
