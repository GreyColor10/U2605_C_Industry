#include "BaseSystem/CPawn.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnhancedInputComponent.h"

#include "Conveyor/CConveyorSubsystem.h"
#include "Component/ActorComponent/CScreenWidgetComponent.h"
#include "Component/CNiagaraComponent.h"

ACPawn::ACPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);

	SpringArmComp->TargetArmLength = 0.0f; 
	SpringArmComp->bUsePawnControlRotation = true; 

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false; 

	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));

	GlobalDataParticleComponent = CreateDefaultSubobject<UCNiagaraComponent>(TEXT("GlobalDataParticleComponent"));
	GlobalDataParticleComponent->SetupAttachment(RootComponent);
	GlobalDataParticleComponent->SetAutoActivate(false);

	ScreenWidgetComponent = CreateDefaultSubobject<UCScreenWidgetComponent>(TEXT("ScreenWidgetComponent"));
}

void ACPawn::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	CheckNotValid(world);

	UCConveyorSubsystem* conveyorSystem = world->GetSubsystem<UCConveyorSubsystem>();
	CheckNotValid(conveyorSystem);

	conveyorSystem->GetOnNiagaraCompActive().AddUObject(this, &ACPawn::OnNiagaraCompActive);
	conveyorSystem->GetOnNiagaraCompSetParticlePosition().AddUObject(this, &ACPawn::OnNiagaraCompSetParticlePosition);
	conveyorSystem->GetOnNiagaraCompSetMeshIndices().AddUObject(this, &ACPawn::OnNiagaraCompSetMeshIndices);
}


void ACPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACPawn::EnhancedMove);
		}

		if (MoveUpAction)
		{
			EnhancedInputComponent->BindAction(MoveUpAction, ETriggerEvent::Triggered, this, &ACPawn::EnhancedMoveUp);
		}
	}

}

void ACPawn::EnhancedMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	CheckNotValid(Controller);

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.X);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void ACPawn::EnhancedMoveUp(const FInputActionValue& Value)
{
	float MoveUpValue = Value.Get<float>();
	if (MoveUpValue != 0.0f)
		AddMovementInput(FVector::UpVector, MoveUpValue);
}

void ACPawn::OnNiagaraCompActive(const bool InBool)
{
	CheckNotValid(GlobalDataParticleComponent);
	GlobalDataParticleComponent->SetActive(InBool);
}

void ACPawn::OnNiagaraCompSetParticlePosition(FName InParaName, const TArray<FVector>& InArrayData)
{
	CheckNotValid(GlobalDataParticleComponent);
	GlobalDataParticleComponent->SetVectorArrayUserParameter(InParaName, InArrayData);
	GlobalDataParticleComponent->SetIntUserParameter(TEXT("ParticleCount"), InArrayData.Num());
}

void ACPawn::OnNiagaraCompSetMeshIndices(FName InParaName, const TArray<int32>& InArrayData)
{
	CheckNotValid(GlobalDataParticleComponent);
	GlobalDataParticleComponent->SetIntArrayUserParameter(InParaName, InArrayData);
}
