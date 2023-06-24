// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestComponent.generated.h"


/**
 * The quest system has the following requirements:
 * 1. Serialization
 * 2. Reusable components to create complex and consistent game behavior.
 * 3. Multi-world travel with no data loss.
 * 4. Dynamic quests and tasks.
 * 5. Networking might get worked on once Flow has good documentation or
 * updated its example project to give an example of multiplayer. As of
 * right now (June 2023) Flow is not very clear on how networking is
 * supposed to work or how it is supposed to be handled.
 *
 * The components that build a quest are the following:
 * 1. The quest itself
 *		- Quests can be failed.
 *		- Quests can have requirements.
 *		- Quests can be repeated.
 *		- Quests can have a time limit.
 * 2. The task to complete the quest
 *		- Some tasks can be optional. Technically all tasks can be optional,
 *		it is up to your system to call "Complete Quest" if you make every
 *		task optional.
 *		- Tasks can be failed.
 *		- Tasks can be dynamically added or removed from a quest.
 *		- Tasks can be reset and repeated.
 *		- Tasks can have a time limit.
 * 3. (Optional) Link the quest to a quest chain.
 * 4. (Optional) A reward or punishment to grant to the player.
 *		- Rewards or punishments can be dynamically added or removed.
 *
 *	Quests, tasks and chains are NOT designed to have multiple copies of
 *	each object being progressed by the player at the same time.
 *	They are all designed to be single objects
 *
 *	Quests are designed to be inside their own flow graph, especially those
 *	that can be repeated, but is not required.
 *	
 *	---
 *
 *	The logic flow is designed to all go through the component, so everything
 *	is easily callable from practically anywhere.
 *
 *	For world traversal, all quest data is stored in a easily modifiable struct
 *	which can be stored in the game instance and then restored once the player
 *	has moved levels.
 *
 *	This system is meant to live as much as possible inside of C++, but can
 *	have blueprint children. It is still advised to store all variables and
 *	functions in C++ and then create a way for Blueprint to work with your
 *	C++ data.
 *
 *	The CDO is heavily used for default values, but then when a quest or task
 *	is started, it is converted into a struct where things can be modified.
 *	Most things are actor components primarily to reduce the amount of
 *	upkeep I'd have to do and utilize all the automatic networking.
 */

/**Things to do:
 * 1. Accept quest
 * 2. Complete quest
 * 3. Drop quest
 * 4. Set active quest
 * 5. Fail quest
 * 6. Get tasks for quest
 *
 * 1. Start task
 * 2. Progress task
 * 3. Fail task
 * 4. Remove task
 * 5. Add/Remove task
 *
 * 1. Accept reward
 * 2. Add/Remove reward
 */

UCLASS(DisplayName = "Core Quest Manager", Blueprintable)
class FLOWEXTENSION_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()
};
