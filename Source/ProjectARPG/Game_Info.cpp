#include "Game_Info.h"

void Test(FString T, float Second, FColor Color)
{
	GEngine->AddOnScreenDebugMessage(-1, Second, Color, T);
}

void Log(FString T)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *T);
}
