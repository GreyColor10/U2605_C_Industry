#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/IClickable.h"
#include "Interface/IProductReceiver.h"
#include "CProductionEquipment_Base.generated.h"

UCLASS(Abstract, NotBlueprintable)
class U2605_C_API ACProductionEquipment_Base 
	: public AActor
	, public IIClickable
	, public IIProductReceiver
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Capsule")
	TObjectPtr<class UCapsuleComponent> CapsuleComp;

	UPROPERTY(EditAnywhere, Category = "UI")
	float UIZOffset = 170.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	TObjectPtr<UStaticMesh> InstancingMesh;

protected:
	UPROPERTY(EditAnywhere, Category = "Equipment")
	FString EquipmentID = TEXT("EQUIP-01");

public:	
	ACProductionEquipment_Base();

protected:
	virtual void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BroadcastInfo() PURE_VIRTUAL(BroadcastInfo,);
	void UITargetBroadcastInfo();

public:
	void OnClicked(const FHitResult& InHit) override;
	virtual void ReceiveProduct(const FProductData& InProductData) override {};
	
	FORCEINLINE const EInfoUIType GetInfoUIType() const override { return InfoUIType; };
	FORCEINLINE const float GetUIZOffset() const override { return UIZOffset; };
	FORCEINLINE UStaticMesh* GetInstancingMesh() const override { return InstancingMesh; };

protected:
	int32 HISMInstanceIndex = -1;
	EInfoUIType InfoUIType = EInfoUIType::None;
};
