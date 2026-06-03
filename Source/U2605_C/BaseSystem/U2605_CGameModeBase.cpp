#include "BaseSystem/U2605_CGameModeBase.h"
#include "Global.h"

#include "Widget/InfoUI/CInfoUIActor.h"

void AU2605_CGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    CheckNull(InfoUIActorClass);

    FActorSpawnParameters params;
    params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    InfoUIActor = GetWorld()->SpawnActor<ACInfoUIActor>(InfoUIActorClass, FVector::ZeroVector, FRotator::ZeroRotator, params);
}
