#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "U2605_CGameModeBase.generated.h"

UCLASS()
class U2605_C_API AU2605_CGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class ACInfoUIActor> InfoUIActorClass;

    UPROPERTY()
    TObjectPtr<class ACInfoUIActor> InfoUIActor;

public:
    FORCEINLINE ACInfoUIActor* GetInfoUIActor() const { return InfoUIActor; }

protected:
    void BeginPlay() override;
};
