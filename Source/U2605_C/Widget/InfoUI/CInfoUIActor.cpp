#include "Widget/InfoUI/CInfoUIActor.h"
#include "Global.h"
#include "GameFramework/PlayerController.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"

#include "Widget/InfoUI/CUserWidget_Info_Storage.h"
#include "Widget/InfoUI/CUserWidget_Info_Processor.h"
#include "Interface/IClickable.h"
#include "Communication/CCommunicationSubsystem_UI.h"

ACInfoUIActor::ACInfoUIActor()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACInfoUIActor::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* playerCon = GetWorld()->GetFirstPlayerController();
    CheckNotValid(playerCon);

    if (StorageWidgetClass)
    {
        StorageWidget = CreateWidget<UCUserWidget_Info_Storage>(playerCon, StorageWidgetClass);
        if (IsValid(StorageWidget))
        {
            StorageWidget->AddToViewport();
            StorageWidget->SetVisibility(ESlateVisibility::Collapsed);
        }   
    }

    if (ProcessorWidgetClass)
    {
        ProcessorWidget = CreateWidget<UCUserWidget_Info_Processor>(playerCon, ProcessorWidgetClass);
        if (IsValid(ProcessorWidget))
        {
            ProcessorWidget->AddToViewport();
            ProcessorWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    SetActorTickEnabled(false);

    UWorld* world = GetWorld();
    CheckNotValid(world);

    FActorSpawnParameters params;
    params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    OutlineActor = world->SpawnActor<AStaticMeshActor>(
        AStaticMeshActor::StaticClass(),
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        params
    );

    if (IsValid(OutlineActor))
    {
        UStaticMeshComponent* outlineMeshComp = OutlineActor->GetStaticMeshComponent();
        if (IsValid(outlineMeshComp))
        {
            outlineMeshComp->SetMobility(EComponentMobility::Movable);
            outlineMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
        OutlineActor->SetActorHiddenInGame(true);
    }

    UGameInstance* game = world->GetGameInstance();
    CheckNotValid(game);

    UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
    CheckNotValid(uiSubsystem);

    uiSubsystem->GetOnUITargetChangedDel().BindUObject(this, &ACInfoUIActor::SetTarget);
    uiSubsystem->GetOnUITargetGotten().BindUObject(this, &ACInfoUIActor::GetTarget);
}

void ACInfoUIActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateWidgetPosition();
}

void ACInfoUIActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UGameInstance* game = GetGameInstance();
    if (IsValid(game))
    {
        UCCommunicationSubsystem_UI* uiSubsystem = game->GetSubsystem<UCCommunicationSubsystem_UI>();
        if (IsValid(uiSubsystem))
        {
            uiSubsystem->GetOnUITargetChangedDel().Unbind();
            uiSubsystem->GetOnUITargetGotten().Unbind();
        }
    }
    Super::EndPlay(EndPlayReason);
}

void ACInfoUIActor::SetTarget(AActor* InActor)
{
    if (TargetActor == InActor)
    {
        Hide();
        return;
    }

    Hide();
    TargetActor = InActor;

    IIClickable* clickable = Cast<IIClickable>(TargetActor.Get());
    CheckNull(clickable);

    EInfoUIType uiType = clickable->GetInfoUIType();
    switch (uiType)
    {
    case EInfoUIType::None:
        ActiveWidget = nullptr;
        break;

    case EInfoUIType::Storage:
        ActiveWidget = StorageWidget;
        break;

    case EInfoUIType::Processor:
        ActiveWidget = ProcessorWidget;
        break;
    }

    CheckNotValid(ActiveWidget);
    ActiveWidget->SetVisibility(ESlateVisibility::Visible);
    SetActorTickEnabled(true);
    UpdateOutlineActor(InActor);
}

void ACInfoUIActor::Hide()
{
    if (ActiveWidget)
        ActiveWidget->SetVisibility(ESlateVisibility::Collapsed);

    TargetActor = nullptr;
    ActiveWidget = nullptr;
    SetActorTickEnabled(false);
    HideOutlineActor();
}

const AActor* ACInfoUIActor::GetTarget() const
{
    CheckFalseResult(TargetActor.IsValid(), nullptr);
    return TargetActor.Get();
}

void ACInfoUIActor::UpdateOutlineActor(AActor* InActor)
{
    CheckNotValid(InActor);
    CheckNotValid(OutlineActor);

    IIClickable* clickable = Cast<IIClickable>(InActor);
    CheckNull(clickable);

    UStaticMesh* mesh = clickable->GetInstancingMesh();
    CheckNull(mesh);

    OutlineActor->SetActorTransform(InActor->GetActorTransform());

    UStaticMeshComponent* outlineMeshComp = OutlineActor->GetStaticMeshComponent();
    CheckNull(outlineMeshComp);

    outlineMeshComp->SetStaticMesh(mesh);

    if (OutlineMaterial)
        outlineMeshComp->SetMaterial(0, OutlineMaterial);

    OutlineActor->SetActorHiddenInGame(false);
}

void ACInfoUIActor::HideOutlineActor()
{
    CheckNotValid(OutlineActor);
    OutlineActor->SetActorHiddenInGame(true);
}

void ACInfoUIActor::UpdateWidgetPosition()
{
    CheckNull(ActiveWidget);
    CheckFalse(TargetActor.IsValid());

    APlayerController* playerCon = GetWorld()->GetFirstPlayerController();
    CheckNotValid(playerCon);

    IIClickable* clickable = Cast<IIClickable>(TargetActor.Get());
    CheckNull(clickable);

    float zPosition = clickable->GetUIZOffset();
    
    FVector worldPos = TargetActor->GetActorLocation() + FVector(0, 0, zPosition);
    FVector2D screenPos;

    bool bProjected = playerCon->ProjectWorldLocationToScreen(worldPos, screenPos);
    CheckFalse(bProjected);

    float dpiScale = GetWorld()->GetGameViewport()->GetDPIScale();
    screenPos /= dpiScale;

    FVector2D widgetSize = ActiveWidget->GetDesiredSize();
    screenPos.X -= widgetSize.X / 2.f;
    screenPos.Y -= widgetSize.Y;

    ActiveWidget->SetPositionInViewport(screenPos, true);
}