// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FE_QuestData.h"
#include "GameplayTagContainer.h"
#include "Nodes/FN_QuestBase.h"
#include "QuestSubSystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestStateUpdated, FQuestWrapper, Quest, TEnumAsByte<EQuestState>, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestDropped, FQuestWrapper, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestFailed, FQuestWrapper, Quest);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestChainStarted, TSoftObjectPtr<UDA_QuestChain>, QuestChain);

/**@Task the task that was progressed.
 * @ProgressMade the delta of the current and the added progress. This can be negative.
 * @Instigator The object that progressed the quest.*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTaskProgressed, FTaskWrapper, Task, float, ProgressMade, UObject*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTaskDropped, FTaskWrapper, Task);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTaskFailed, FTaskWrapper, Task);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTaskAddedToQuest, FTaskWrapper, Task, FQuestWrapper, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTaskRemovedFromQuest, FTaskWrapper, Task, FQuestWrapper, Quest);


UCLASS(DisplayName = "Core Quest Manager", Blueprintable, meta = (BlueprintSpawnableComponent))
class FLOWEXTENSION_API UQuestSubSystem : public UGameInstanceSubsystem, public II_QuestUpdates
{
	GENERATED_BODY()

 UQuestSubSystem();

public:

 UPROPERTY(Category = "Quest", BlueprintReadOnly, SaveGame)
 TMap<TSoftObjectPtr<UDA_Quest>, FQuestWrapper> Quests;

 UPROPERTY(Category = "Quest", BlueprintReadOnly, SaveGame)
 TSet<TSoftObjectPtr<UDA_QuestChain>> QuestChains;

 //Delegates

 UPROPERTY(Category = "Quest", BlueprintAssignable)
 FQuestStateUpdated QuestStateUpdated;
 
 UPROPERTY(Category = "Quest", BlueprintAssignable)
 FQuestDropped QuestDropped;

 UPROPERTY(Category = "Quest", BlueprintAssignable)
 FQuestFailed QuestFailed;

 UPROPERTY(Category = "Quest", BlueprintAssignable)
 FQuestChainStarted QuestChainStarted;


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

 static UQuestSubSystem* Get();
 
 //------------------
 // Quest

 UFUNCTION(Category = "Quest", BlueprintCallable)
 static void AddListenerToQuest(TSoftObjectPtr<UDA_Quest> Quest, UObject* Listener);

 /**Accept a quest from a node.
  * Will only return true if the quest was accepted,
  * if it returns false it means the player has already
  * completed it or has it.
  * @ForceAccept if true, we skip CanAcceptQuest() */
 UFUNCTION(Category = "Quest", BlueprintCallable)
 static bool AcceptQuest(TSoftObjectPtr<UDA_Quest> Quest, bool ForceAccept = false);

 UFUNCTION(Category = "Quest", BlueprintPure)
 static bool CanAcceptQuest(TSoftObjectPtr<UDA_Quest> Quest);

 /**Complete the quest.
  *
  * @SkipCompletionCheck Typically, you want CanCompleteQuest to be called,
  * but sometimes you want to forcibly complete the quest.
  * @AutoAcceptQuest If true, we will accept the quest (forcefully) if the
  * quest hasn't already been accepted.*/
 UFUNCTION(Category = "Quest", BlueprintCallable)
 static void CompleteQuest(TSoftObjectPtr<UDA_Quest> Quest, bool SkipCompletionCheck = false, bool AutoAcceptQuest = true);
 
 UFUNCTION(Category = "Quest", BlueprintPure)
 static bool CanCompleteQuest(FQuestWrapper Quest);

 UFUNCTION(Category = "Quest", BlueprintPure)
 static bool HasCompletedQuest(TSoftObjectPtr<UDA_Quest> Quest);

 UFUNCTION(Category = "Quest", BlueprintPure)
 static bool HasFailedQuest(TSoftObjectPtr<UDA_Quest> Quest);

 UFUNCTION(Category = "Quest", BlueprintPure)
 static TEnumAsByte<EQuestState> GetQuestState(TSoftObjectPtr<UDA_Quest> Quest);

 UFUNCTION(Category = "Quest", BlueprintCallable)
 static bool DropQuest(TSoftObjectPtr<UDA_Quest> Quest);

 /**Attempt to fail the quest, only returns false if the quest is
  * not in progress.
  *
  * @FailTasks Whether or not the tasks should be labelled as "failed"
  * and if we should notify its listeners and broadcast the
  * failure delegate.*/
 UFUNCTION(Category = "Quest", BlueprintCallable)
 static bool FailQuest(TSoftObjectPtr<UDA_Quest> Quest, bool FailTasks);

 UFUNCTION(Category = "Quest", BlueprintCallable)
 static TArray<FQuestWrapper> GetQuestsWithState(TEnumAsByte<EQuestState> State);

 //------------------


 //------------------
 // Quest Chain

 UFUNCTION(Category = "Quest Chain", BlueprintPure)
 static TArray<TSoftObjectPtr<UDA_Quest>> GetRequiredQuestsForQuest(TSoftObjectPtr<UDA_Quest> Quest);
 
 /**Resolve whether the required quests have been completed for the @Quest.
 * Keep in mind, this will load the quest synchronously.*/
 UFUNCTION(Category = "Quest Chain", BlueprintPure)
 static bool HasCompletedRequiredQuests(TSoftObjectPtr<UDA_Quest> Quest);

 UFUNCTION(Category = "Quest Chain", BlueprintPure)
 static int32 GetCurrentStageOnQuestChain(TSoftObjectPtr<UDA_QuestChain> QuestChain);

 /**Get the percentage progress of this quest chain (0 - 1)*/
 UFUNCTION(Category = "Quest Chain", BlueprintPure)
 static float GetQuestChainProgress(TSoftObjectPtr<UDA_QuestChain> QuestChain);

 //------------------
 

 //------------------
 // Task

 /**Search the active quests for the task.
  * TODO: Investigate if we can get away with having a TMap
  * of task tags with their quest associated with it, so we
  * can get the quest instantly (if this function is slow)*/
 UFUNCTION(Category = "Quest", BlueprintPure)
 static FQuestWrapper GetQuestForTask(FGameplayTag Task);
 
 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintPure)
 static FTaskWrapper GetTaskWrapper(FGameplayTag Task);

 UFUNCTION(Category = "Quest", BlueprintPure)
 static TArray<FTaskWrapper> GetTasksForQuest(TSoftObjectPtr<UDA_Quest> Quest);

 UFUNCTION(Category = "Quest", BlueprintCallable)
 static void AddListenerToTask(FGameplayTag Task, UObject* Listener);

 /**Add or deduct progress to a task.
  * @Instigator Who is attempting to progress the task?*/
 UFUNCTION(Category = "Quest", BlueprintCallable)
 static bool ProgressTask(const FGameplayTag Task, float ProgressToAdd, UObject* Instigator);

 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintPure)
 static float GetTaskProgress(FGameplayTag Task);

 UFUNCTION(Category = "Quest", BlueprintCallable, BlueprintPure)
 static TEnumAsByte<EQuestState> GetTaskState(FGameplayTag Task);

 UFUNCTION(Category = "Quest", BlueprintCallable)
 static bool CompleteTask(FGameplayTag Task, UObject* Instigator);

 /**Evaluate if the task can be progressed.
  *
  * This will also send the I_QuestUpdates -> PreventTaskProgress interface
  * event to this component AND the listeners of the task. Allowing any
  * UObject to prevent the tasks progress, for example a task might
  * be to kill a boss. If the boss is a listener for the task, you can
  * ask the boss if they are dead or not, if the boss isn't dead,
  * you can prevent the task from being progressed.*/
 UFUNCTION(Category = "Quest", BlueprintCallable)
 static bool CanTaskBeProgressed(FGameplayTag Task);

 /**Attempt to fail a task.
  *
  * @FailQuest Whether or not the entire quest this task belongs to
  * should also be failed.*/
 UFUNCTION(Category = "Quest", BlueprintCallable)
 static bool FailTask(FGameplayTag Task, bool bFailQuest);

 /**Adds a task to a quest, this is not generally advised to use,
  * since the design process is supposed to be all within the quest
  * flow node.
  * The better way would be to have tasks that are hidden from the
  * player, then revealed once you want to "add" the task to the
  * quest.
  * Also note: If you try and add a task AFTER all the tasks in the
  * quest have been completed, the quest will still get labelled as
  * completed.*/
 UFUNCTION(Category = "Quest", BlueprintCallable, meta = (DeprecatedFunction = "Unsupported feature, was just complicated to QA around"))
 static bool AddTaskToQuest(FQuestTask Task, TSoftObjectPtr<UDA_Quest> Quest);

 /**Attempt to remove a task from a quest.
  * If there's no tasks left, the quest will be dropped.*/
 UFUNCTION(Category = "Quest", BlueprintCallable, meta = (DeprecatedFunction = "Unsupported feature, was just complicated to QA around"))
 static bool RemoveTaskFromQuest(FGameplayTag Task, TSoftObjectPtr<UDA_Quest> Quest);

 //------------------


 //------------------
 // Editor

 static void SetQuestState(const TArray<FString>& Args);

 //------------------
};
