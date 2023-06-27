// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Nodes/FN_QuestBase.h"
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
 *	It is also meant to live on the player controller
 *
 * For quests to be progressed, the graph owning the quest node MUST be alive.
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestStateUpdated, FS_QuestWrapper, Quest, TEnumAsByte<EQuestState>, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestDropped, FS_QuestWrapper, Quest);

/**@Task the task that was progressed.
 * @ProgressMade the delta of the current and the added progress. This can be negative.
 * @Instigator The object that progressed the quest.*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTaskProgressed, FS_TaskWrapper, Task, float, ProgressMade, UObject*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTaskDropped, FS_TaskWrapper, Task);


UCLASS(DisplayName = "Core Quest Manager", Blueprintable, meta = (BlueprintSpawnableComponent))
class FLOWEXTENSION_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:

 UPROPERTY(Category = "Quest", BlueprintReadOnly, SaveGame)
 TArray<FS_QuestWrapper> ActiveQuests;

 UPROPERTY(Category = "Quest", BlueprintReadOnly, SaveGame)
 TArray<FS_QuestWrapper> CompletedQuests;

 UPROPERTY(Category = "Quest", BlueprintReadOnly, SaveGame)
 TArray<FS_QuestWrapper> FailedQuests;

 //Delegates

 UPROPERTY(Category = "Quest", BlueprintAssignable)
 FQuestStateUpdated QuestStateUpdated;
 
 UPROPERTY(Category = "Quest", BlueprintAssignable)
 FQuestDropped QuestDropped;

 UPROPERTY(Category = "Task", BlueprintAssignable)
 FTaskProgressed TaskProgressed;

 UPROPERTY(Category = "Task", BlueprintAssignable)
 FTaskDropped TaskDropped;
 
 
 //------------------
 // Quest

 UFUNCTION(Category = "Quest", BlueprintCallable)
 void AddListenerToQuest(FGameplayTag Quest, UObject* Listener);

 UFUNCTION(Category = "Quest", BlueprintPure, meta = (DisplayName = "Get Quest Wrapper (Active)"))
 FS_QuestWrapper GetQuestWrapper_Active(FGameplayTag Quest);

 /**Accept a quest from a node.
  * Will only return true if the quest was accepted,
  * if it returns false it means the player has already
  * completed it or has it.*/
 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 bool AcceptQuest(UFN_QuestBase* Quest);

 UFUNCTION(Category = "Quest", BlueprintPure, BlueprintNativeEvent)
 bool CanAcceptQuest(FGameplayTag Quest);

 /**Get the index of the quest from the active quests array.
  * Can return -1 if the quest is not found.*/
 UFUNCTION(Category = "Quest", BlueprintPure)
 int32 GetQuestIndex_Active(FGameplayTag Quest);

 /**Complete the quest.
  * @SkipCompletionCheck Typically you want CanCompleteQuest to be called,
  * but sometimes you want to forcibly complete the quest.*/
 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 void CompleteQuest(FS_QuestWrapper Quest, bool SkipCompletionCheck);
 
 UFUNCTION(Category = "Quest", BlueprintPure, BlueprintNativeEvent)
 bool CanCompleteQuest(FS_QuestWrapper Quest);

 UFUNCTION(Category = "Quest", BlueprintPure)
 bool HasCompletedQuest(FGameplayTag Quest);

 UFUNCTION(Category = "Quest", BlueprintPure)
 bool IsQuestActive(FGameplayTag Quest);

 UFUNCTION(Category = "Quest", BlueprintPure)
 bool HasFailedQuest(FGameplayTag Quest);

 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 bool DropQuest(FS_QuestWrapper Quest);

 //------------------


 //------------------
 // Task

 /**Search the active quests for the task.
  * @ArrayIndex can return -1 if the quest was not found.*/
 UFUNCTION(Category = "Quest", BlueprintPure)
 FS_QuestWrapper GetQuestForTask_Active(FGameplayTag Task, int32& ArrayIndex);

 UFUNCTION(Category = "Quest", BlueprintCallable)
 void AddListenerToTask(FGameplayTag Task, UObject* Listener);

 /**Add or deduct progress to a task.
  * @Instigator Who is attempting to progress the task?*/
 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 bool ProgressTask(const FGameplayTag Task, float ProgressToAdd, UObject* Instigator);

 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 bool CanTaskBeProgressed(const FGameplayTag Task);

 /**Small optimization, CanTaskBeProgressed already looks up
  * the task, overriding that function at a blueprint level means
  * you have to search for the task again. This will automatically
  * receive the already-found task.*/
 UFUNCTION(Category = "Quest", BlueprintNativeEvent, DisplayName = "Can task be progressed?")
 bool CanTaskBeProgressed_Internal(FS_TaskWrapper Task);

 //------------------
 
};
