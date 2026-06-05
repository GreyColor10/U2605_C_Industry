#include "BaseSystem/CPlayerController.h"
#include "Global.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Interface/IClickable.h"
#include "BaseSystem/U2605_CGameModeBase.h"
#include "Widget/InfoUI/CInfoUIActor.h"
#include "ProductionEquipment/ProductionEquipment_Base/CProductionEquipment_Storage.h"

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

		AGameModeBase* gameMode = world->GetAuthGameMode();
		CheckNotValid(gameMode);

		AU2605_CGameModeBase* cGameMode = Cast<AU2605_CGameModeBase>(gameMode);
		CheckNotValid(cGameMode);

		ACInfoUIActor* infoUI = cGameMode->GetInfoUIActor();
		if (IsValid(infoUI)) infoUI->SetTarget(hitActor);

		//초기화 순서 문제가 있어, 나중에 호출함
		clickable->OnClicked(HitResult);

		return;
	}

	else
	{
		// 디버그: 비어있지 않은 첫 Storage 출고
		TArray<ACProductionEquipment_Storage*> storages;
		FHelpers::FindActors<ACProductionEquipment_Storage>(world, storages);

		for (ACProductionEquipment_Storage* storage : storages)
		{
			if (!IsValid(storage)) continue;
			if (storage->IsEmpty()) continue;
			if (storage->ShipProduct()) return;
		}

		FLog::Log(TEXT("Click: No storage has products to ship."));
	}
}
