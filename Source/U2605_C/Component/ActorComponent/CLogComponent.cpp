#include "Component/ActorComponent/CLogComponent.h"

UCLogComponent::UCLogComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCLogComponent::BeginPlay()
{
	Super::BeginPlay();
	
}