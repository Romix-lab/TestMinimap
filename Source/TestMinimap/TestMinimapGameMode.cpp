// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestMinimapGameMode.h"
#include "TestMinimapCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATestMinimapGameMode::ATestMinimapGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
