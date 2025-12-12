#include "Game_Info.h"

void Test(FString T)
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, T);
}

void Log(FString T)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *T);
}
