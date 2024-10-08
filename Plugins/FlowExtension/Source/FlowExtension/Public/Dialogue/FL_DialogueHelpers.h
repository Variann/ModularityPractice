﻿// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FE_DialogueData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FL_DialogueHelpers.generated.h"

/**
 * 
 */
UCLASS()
class FLOWEXTENSION_API UFL_DialogueHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(Category = "Flow Extension|DialogueHelpers", BlueprintCallable, BlueprintPure)
	static bool DoesDialogueMeetCondition(FDialogueOption DialogueOption);
};
