#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

#define ClickAbleChannel ECC_GameTraceChannel1 

struct FInputActionValue;

UCLASS()
class U2605_C_API ACPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MouseClickAction = nullptr;

protected:
	void BeginPlay() override;
	void SetupInputComponent() override;
	void OnMouseClick(const FInputActionValue& Value);
};
