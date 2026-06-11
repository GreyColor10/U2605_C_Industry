#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StructData/CStructDatas.h"
#include "CInfoBoard.generated.h"


UCLASS()
class U2605_C_API ACInfoBoard : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board")
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Board")
	TObjectPtr<class UWidgetComponent> WidgetComponent;

private:
	UFUNCTION()
	void OnDashboardUpdated(const FDashboardData& InDashboardData);

public:	
	ACInfoBoard();

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
