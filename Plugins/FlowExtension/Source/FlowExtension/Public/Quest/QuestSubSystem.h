// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Nodes/FN_QuestBase.h"
#include "QuestSubSystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestStateUpdated, FQuestWrapper, Quest, TEnumAsByte<EQuestState>, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestDropped, FQuestWrapper, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestFailed, FQuestWrapper, Quest);

/**@Task the task that was progressed.
 * @ProgressMade the delta of the current and the added progress. This can be negative.
 * @Instigator The object that progressed the quest.*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTaskProgressed, FTaskWrapper, Task, float, ProgressMade, UObject*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTaskDropped, FTaskWrapper, Task);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTaskFailed, FTaskWrapper, Task);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTaskAddedToQuest, FTaskWrapper, Task, FQuestWrapper, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTaskRemovedFromQuest, FTaskWrapper, Task, FQuestWrapper, Quest);


UCLASS(DisplayName = "Core Quest Manager")
class FLOWEXTENSION_API UQuestSubSystem : public UGameInstanceSubsystem, public II_QuestUpdates
{
	GENERATED_BODY()

public:

 UPROPERTY(Category = "Quest", BlueprintReadOnly, SaveGame)
 TArray<FQuestWrapper> ActiveQuests;

 UPROPERTY(Category = "Quest", BlueprintReadOnly, SaveGame)
 TArray<FQuestWrapper> CompletedQuests;

 UPROPERTY(Category = "Quest", BlueprintReadOnly, SaveGame)
 TArray<FQuestWrapper> FailedQuests;

 //Delegates

 UPROPERTY(Category = "Quest", BlueprintAssignable)
 FQuestStateUpdated QuestStateUpdated;
 
 UPROPERTY(Category = "Quest", BlueprintAssignable)
 FQuestDropped QuestDropped;

 UPROPERTY(Category = "Quest", BlueprintAssignable)
 FQuestFailed QuestFailed;


 UPROPERTY(Category = "Task", BlueprintAssignable)
 FTaskProgressed TaskProgressed;

 UPROPERTY(Category = "Task", BlueprintAssignable)
 FTaskDropped TaskDropped;

 UPROPERTY(Category = "Task", BlueprintAssignable)
 FTaskFailed TaskFailed;
 
 UPROPERTY(Category = "Task", BlueprintAssignable)
 FTaskAddedToQuest TaskAddedToQuest;

 UPROPERTY(Category = "Task", BlueprintAssignable)
 FTaskRemovedFromQuest TaskRemovedFromQuest;
 
 
 //------------------
 // Quest

 UFUNCTION(Category = "Quest", BlueprintCallable)
 void AddListenerToQuest(TSoftObjectPtr<UDA_Quest> Quest, UObject* Listener);

 UFUNCTION(Category = "Quest", BlueprintPure, meta = (DisplayName = "Get Quest Wrapper (Active)"))
 FQuestWrapper GetQuestWrapper_Active(TSoftObjectPtr<UDA_Quest> Quest, int32& ArrayIndex);

 /**Accept a quest from a node.
  * Will only return true if the quest was accepted,
  * if it returns false it means the player has already
  * completed it or has it.*/
 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 bool AcceptQuest(UDA_Quest* Quest);

 UFUNCTION(Category = "Quest", BlueprintPure, BlueprintNativeEvent)
 bool CanAcceptQuest(const TSoftObjectPtr<UDA_Quest>& Quest);

 /**Get the index of the quest from the active quests array.
  * Can return -1 if the quest is not found.*/
 UFUNCTION(Category = "Quest", BlueprintPure)
 int32 GetQuestIndex_Active(const TSoftObjectPtr<UDA_Quest>& Quest);

 /**Complete the quest.
  *
  * @SkipCompletionCheck Typically you want CanCompleteQuest to be called,
  * but sometimes you want to forcibly complete the quest.*/
 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 void CompleteQuest(FQuestWrapper Quest, bool SkipCompletionCheck);
 
 UFUNCTION(Category = "Quest", BlueprintPure, BlueprintNativeEvent)
 bool CanCompleteQuest(FQuestWrapper Quest);

 UFUNCTION(Category = "Quest", BlueprintPure)
 bool HasCompletedQuest(const TSoftObjectPtr<UDA_Quest>& Quest);

 UFUNCTION(Category = "Quest", BlueprintPure)
 bool IsQuestActive(const TSoftObjectPtr<UDA_Quest>& Quest);

 UFUNCTION(Category = "Quest", BlueprintPure)
 bool HasFailedQuest(const TSoftObjectPtr<UDA_Quest>& Quest);

 UFUNCTION(Category = "Quest", BlueprintPure)
 TEnumAsByte<EQuestState> GetQuestState(TSoftObjectPtr<UDA_Quest> Quest);

 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 bool DropQuest(FQuestWrapper Quest);

 /**Attempt to fail the quest, only returns false if the quest is
  * not in progress.
  *
  * @FailTasks Whether or not the tasks should be labelled as "failed"
  * and if we should notify its listeners and broadcast the
  * failure delegate.*/
 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 bool FailQuest(FQuestWrapper Quest, bool FailTasks);

 //------------------


 //------------------
 // Task

 /**Search the active quests for the task.
  * @ArrayIndex can return -1 if the quest was not found.*/
 UFUNCTION(Category = "Quest", BlueprintPure)
 FQuestWrapper GetQuestForTask_Active(FGameplayTag Task, int32& ArrayIndex);

 UFUNCTION(Category = "Quest", BlueprintPure)
 TArray<FTaskWrapper> GetTasksForQuest_Active(const TSoftObjectPtr<UDA_Quest>& Quest);

 UFUNCTION(Category = "Quest", BlueprintCallable)
 void AddListenerToTask(FGameplayTag Task, UObject* Listener);

 /**Add or deduct progress to a task.
  * @Instigator Who is attempting to progress the task?*/
 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 bool ProgressTask(const FGameplayTag Task, float ProgressToAdd, UObject* Instigator);

 /**Evaluate if the task can be progressed.
  *
  * This will also send the I_QuestUpdates -> PreventTaskProgress interface
  * event to this component AND the listeners of the task. Allowing any
  * UObject to prevent the tasks progress, for example a task might
  * be to kill a boss. If the boss is a listener for the task, you can
  * ask the boss if they are dead or not, if the boss isn't dead,
  * you can prevent the task from being progressed.*/
 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 bool CanTaskBeProgressed(FTaskWrapper Task);

 /**Small optimization, CanTaskBeProgressed already looks up
  * the task, overriding that function at a blueprint level means
  * you have to search for the task again. This will automatically
  * receive the already-found task.*/
 // UFUNCTION(Category = "Quest", BlueprintNativeEvent, DisplayName = "Can task be progressed?")
 // bool CanTaskBeProgressed_Internal(FS_TaskWrapper Task);

 /**Attempt to fail a task.
  *
  * @FailQuest Whether or not the entire quest this task belongs to
  * should also be failed.*/
 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 bool FailTask(FGameplayTag Task, bool FailQuest);

 /**Adds a task to a quest, this is not generally advised to use,
  * since the design process is supposed to be all within the quest
  * flow node.
  * The better way would be to have tasks that are hidden from the
  * player, then revealed once you want to "add" the task to the
  * quest.
  * Also note: If you try and add a task AFTER all the tasks in the
  * quest have been completed, the quest will still get labelled as
  * completed.*/
 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 bool AddTaskToQuest(FQuestTask Task, TSoftObjectPtr<UDA_Quest> Quest);

 /**Attempt to remove a task from a quest.
  * If there's no tasks left, the quest will be dropped.*/
 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintAuthorityOnly)
 bool RemoveTaskFromQuest(FGameplayTag Task, TSoftObjectPtr<UDA_Quest> Quest);

 //------------------
 
};
