#include "Utilities/FLog.h"
#include "Engine.h"

DEFINE_LOG_CATEGORY_STATIC(Game, Display, All)

void FLog::Log(int32 InValue)
{
	//GLog->Logf(L"%d", InValue);

	UE_LOG(Game, Display, L"%d", InValue);
}

void FLog::Log(float InValue)
{
	UE_LOG(Game, Display, L"%f", InValue);
}

void FLog::Log(const FString& InValue)
{
	UE_LOG(Game, Display, L"%s", *InValue);
}

void FLog::Log(const FVector& InValue)
{
	UE_LOG(Game, Display, L"%s", *InValue.ToString());
}

void FLog::Log(const FRotator& InValue)
{
	UE_LOG(Game, Display, L"%s", *InValue.ToString());
}

void FLog::Log(const UObject* InValue)
{
	FString str;

	if (!!InValue)
		str.Append(InValue->GetName());

	str.Append(!!InValue ? " Not Null" : "Null");

	UE_LOG(Game, Display, L"%s", *str);
}

void FLog::Print(int32 InValue, int32 InKey, float InDuration, const FColor& InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, InDuration, InColor, FString::FromInt(InValue));
}

void FLog::Print(float InValue, int32 InKey, float InDuration, const FColor& InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, InDuration, InColor, FString::SanitizeFloat(InValue));
}

void FLog::Print(const FString& InValue, int32 InKey, float InDuration, const FColor& InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, InDuration, InColor, InValue);
}

void FLog::Print(const FVector& InValue, int32 InKey, float InDuration, const FColor& InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, InDuration, InColor, InValue.ToString());
}

void FLog::Print(const FRotator& InValue, int32 InKey, float InDuration, const FColor& InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, InDuration, InColor, InValue.ToString());
}

void FLog::Print(const UObject* InValue, int32 InKey, float InDuration, const FColor& InColor)
{
	FString str;

	if (!!InValue)
		str.Append(InValue->GetName());
		/*str.Append(FString::Printf(TEXT("%x"), InValue));*/

	str.Append(!!InValue ? " Not Null" : "Null");

	GEngine->AddOnScreenDebugMessage(InKey, InDuration, InColor, str);

}

