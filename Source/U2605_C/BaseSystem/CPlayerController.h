#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

#define ClickAbleChannel ECC_GameTraceChannel1 

struct FInputActionValue;
class ACineCameraActor;

UCLASS()
class U2605_C_API ACPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* DefaultMappingContext = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MouseClickAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	class UInputAction* Camera0Action = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	class UInputAction* Camera1Action = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	class UInputAction* Camera2Action = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input|Camera")
	class UInputAction* Camera3Action = nullptr;

	UPROPERTY(EditAnywhere, Category = "Camera")
	TArray<FName> CineCameraTags;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float ViewBlendTime = 0.4f;

	UPROPERTY()
	TArray<TObjectPtr<ACineCameraActor>> CineCameras;

protected:
	void BeginPlay() override;
	void SetupInputComponent() override;

private:
	void OnMouseClick(const FInputActionValue& Value);

	void OnCamera0(const FInputActionValue& Value);
	void OnCamera1(const FInputActionValue& Value);
	void OnCamera2(const FInputActionValue& Value);
	void OnCamera3(const FInputActionValue& Value);

	void SwitchToCameraByIndex(int32 Index);
	void SwitchToCamera(AActor* Target);

private:
	TWeakObjectPtr<AActor> DefaultViewTarget;
};
