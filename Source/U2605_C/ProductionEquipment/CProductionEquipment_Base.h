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
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capsule")
	TObjectPtr<class UCapsuleComponent> CapsuleComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMesh> InstancingMesh;

	// 최대 보관 수 (0 이하 = 무제한)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Storage")
	int32 MaxCapacity = 0;

	// 런타임 보관 목록
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Storage|Runtime")
	TArray<FProductData> StoredProducts;

public:
	UFUNCTION(BlueprintCallable, Category = "Storage")
	bool ShipProduct();

	UFUNCTION(BlueprintPure, Category = "Storage")
	bool IsFull() const;

	UFUNCTION(BlueprintPure, Category = "Storage")
	FORCEINLINE int32 GetStoredCount() const { return StoredProducts.Num(); }

	UFUNCTION(BlueprintPure, Category = "Storage")
	FORCEINLINE bool IsEmpty() const { return StoredProducts.IsEmpty(); }

public:	
	ACProductionEquipment_Base();

protected:
	virtual void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void ReceiveProduct(const FProductData& InProductData) override {}

protected:
	int32 HISMInstanceIndex = -1;
};
