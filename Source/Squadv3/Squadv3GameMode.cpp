// Copyright Epic Games, Inc. All Rights Reserved.

#include "Squadv3GameMode.h"
#include "Squadv3Character.h"
#include "UObject/ConstructorHelpers.h"

ASquadv3GameMode::ASquadv3GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
