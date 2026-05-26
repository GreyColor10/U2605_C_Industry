#pragma once

#include "CoreMinimal.h"

#define CheckTrue(x) do { if(x == true) return; } while(0)
#define CheckTrueResult(x, y) do { if(x == true) return y; } while(0)

#define CheckFalse(x) do { if(x == false) return; } while(0)
#define CheckFalseResult(x, y) do { if(x == false) return y; } while(0)

#define CheckNull(x) do { if(x == nullptr) return; } while(0)
#define CheckNullResult(x, y) do { if(x == nullptr) return y; } while(0)

#define CheckNotValid(x) do { if(!IsValid(x)) return; } while(0)
#define CheckNotValidResult(x, y) do { if(!IsValid(x)) return y; } while(0)

#define CreateTextRenderComponent()\
{\
	FHelpers::CreateComponent<UTextRenderComponent>(this, &Text, "Text", Root);\
	Text->SetRelativeLocation(FVector(0, 0, 100));\
	Text->SetRelativeRotation(FRotator(0, 180, 0));\
	Text->SetRelativeScale3D(FVector(2));\
	Text->HorizontalAlignment = EHorizTextAligment::EHTA_Center;\
	Text->TextRenderColor = FColor::Red;\
	Text->Text = FText::FromString(GetName().Replace(L"Default__", L""));\
}

class U2605_C_API FHelpers
{
public:
	template<typename TValueType>
	static void CreateComponent(AActor* InActor, TValueType** OutComponent, FName InName, USceneComponent* InParent = nullptr)
	{
		*OutComponent = InActor->CreateDefaultSubobject<TValueType>(InName);

		USceneComponent* sceneComp = Cast<USceneComponent>(*OutComponent);
		CheckNotValid(sceneComp);

		if (InParent)
		{
			sceneComp->SetupAttachment(InParent);

			return;
		}

		InActor->SetRootComponent(sceneComp);
	}

	template<typename TValueType>
	static void CreateActorComponent(AActor* InActor, TValueType** OutComponent, FName InName)
	{
		*OutComponent = InActor->CreateDefaultSubobject<TValueType>(InName);
	}

	template<typename TValueType>
	static void GetAsset(TValueType** OutObject, FString InPath)
	{
		ConstructorHelpers::FObjectFinder<TValueType> asset(*InPath);
		*OutObject = asset.Object;
	}

	template<typename TValueType>
	static void GetAssetDynamic(TValueType** OutObject, FString InPath)
	{
		*OutObject = Cast<TValueType>(StaticLoadObject(TValueType::StaticClass(), nullptr, *InPath));
	}

	template<typename TClassName>
	static void GetClass(TSubclassOf<TClassName>* OutClass, FString InPath)
	{
		ConstructorHelpers::FClassFinder<TClassName> asset(*InPath);
		*OutClass = asset.Class;
	}

	template<typename TFindType>
	static TFindType* FindActor(UWorld* InWorld)
	{
		for (AActor* actor : InWorld->GetCurrentLevel()->Actors)
		{
			if (!!actor && actor->IsA<TFindType>())
				return Cast<TFindType>(actor);
		}

		return nullptr;
	}

	template<typename TFindType>
	static void FindActors(UWorld* InWorld, TArray<TFindType*>& OutActors)
	{
		for (AActor* actor : InWorld->GetCurrentLevel()->Actors)
		{
			if (!!actor && actor->IsA<TFindType>())
				OutActors.Add(Cast<TFindType>(actor));
		}
	}

	template<typename TFindType>
	static TFindType* GetComponent(AActor* InActor)
	{
		return Cast<TFindType>(InActor->GetComponentByClass(TFindType::StaticClass()));
	}

	static void AttachTo(AActor* InActor, USceneComponent* InParent, FName InSocketName = NAME_None, EAttachmentRule InRule = EAttachmentRule::KeepRelative)
	{
		InActor->AttachToComponent(InParent, FAttachmentTransformRules(InRule, true), InSocketName);
	}
};