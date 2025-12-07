// Copyright Epic Games, Inc. All Rights Reserved.

#include "MatchSystemGameMode.h"
#include "MatchSystemCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMatchSystemGameMode::AMatchSystemGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}


void AMatchSystemGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("A client joined: %s"), *NewPlayer->GetName());

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Green,
			FString::Printf(TEXT("A player joined: %s"), *NewPlayer->GetName())
		);
	}
}