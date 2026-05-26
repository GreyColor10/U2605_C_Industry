#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/IClickable.h"
#include "CProductionEquipment_Base.generated.h"

UCLASS()
class U2605_C_API ACProductionEquipment_Base 
	: public AActor
	, public IIClickable

{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capsule")
	TObjectPtr<class UCapsuleComponent> CapsuleComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMesh> InstancingMesh;

public:	
	ACProductionEquipment_Base();

protected:
	void BeginPlay() override;

};
