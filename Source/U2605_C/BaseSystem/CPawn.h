#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CPawn.generated.h"

struct FInputActionValue;

UCLASS()
class U2605_C_API ACPawn : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capsule")
	TObjectPtr<class UCapsuleComponent> CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<class UFloatingPawnMovement> MovementComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveUpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Niagara")
	TObjectPtr<class UCNiagaraComponent> GlobalDataParticleComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TObjectPtr<class UCScreenWidgetComponent> ScreenWidgetComponent = nullptr;

public:
	ACPawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void EnhancedMove(const FInputActionValue& Value);
	void EnhancedMoveUp(const FInputActionValue& Value);

public:
	void OnNiagaraCompActive(const bool InBool);
	void OnNiagaraCompSetParticlePosition(FName InParaName, const TArray<FVector>& InArrayData);
	void OnNiagaraCompSetMeshIndices(FName InParaName, const TArray<int32>& InArrayData);
};
