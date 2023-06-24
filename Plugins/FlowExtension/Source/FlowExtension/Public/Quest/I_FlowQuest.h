// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_FlowQuest.generated.h"

class UQuestComponent;
// This class does not need to be modified.
UINTERFACE()
class UI_FlowQuest : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FLOWEXTENSION_API II_FlowQuest
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/**Quest component should always live on the player controller, this can
	 * help retrieve the correct quest component in a multiplayer session
	 * from a specific actor.*/
	UFUNCTION(Category = "Quest System", BlueprintNativeEvent, BlueprintCallable)
	void GetQuestComponent(UQuestComponent*& QuestComponent);
};
