// Copyright Epic Games, Inc. All Rights Reserved.

#include "ModularityPracticeGameMode.h"
#include "ModularityPracticeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AModularityPracticeGameMode::AModularityPracticeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
