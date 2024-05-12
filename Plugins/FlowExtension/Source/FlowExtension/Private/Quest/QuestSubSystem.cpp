// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestSubSystem.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Quest/FL_QuestHelpers.h"
#include "Quest/I_QuestUpdates.h"
#include "VisualLogger/VisualLoggerKismetLibrary.h"
#include "Math/Color.h"


void UQuestSubSystem::AddListenerToQuest(const TSoftObjectPtr<UDA_Quest> Quest, UObject* Listener)
{
	FQuestWrapper* QuestWrapper = Quests.Find(Quest);
	if(QuestWrapper)
	{
		QuestWrapper->Listeners.Add(Listener);
	}
}

bool UQuestSubSystem::AcceptQuest(UDA_Quest* Quest)
{
	if(!IsValid(Quest))
	{
		return false;
	}
	
	if(!CanAcceptQuest(Quest))
	{
		return false;
	}

	//Player can accept the quest, start accepting it.

	//Wrap the quest into a struct that is more easily
	//serialized and manageable.
	FQuestWrapper QuestWrapper = UFL_QuestHelpers::WrapQuest(Quest);

	//Wrap the tasks into a struct that is more easily
	//serialized and manageable.
	for(const auto& CurrentTask : Quest->Tasks)
	{
		FTaskWrapper TaskWrapper = UFL_QuestHelpers::WrapTask(Quest, CurrentTask);
		
		QuestWrapper.Tasks.Add(TaskWrapper);
	}

	Quests.Add(Quest, QuestWrapper);
	QuestStateUpdated.Broadcast(QuestWrapper, InProgress);

	#if ENABLE_VISUAL_LOG
	
	UE_VLOG_LOCATION(this, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(),
		10, FColor::White, TEXT("Accepted Quest: %s"), *UKismetSystemLibrary::GetObjectName(Quest));

	#endif
	
	return true;
}

bool UQuestSubSystem::CanAcceptQuest_Implementation(const TSoftObjectPtr<UDA_Quest>& Quest)
{
	if(HasCompletedQuest(Quest))
	{
		return false;
	}

	if(GetQuestState(Quest) == InProgress)
	{
		return false;
	}

	if(HasFailedQuest(Quest))
	{
		return false;
	}

	return true;
}

void UQuestSubSystem::CompleteQuest(FQuestWrapper Quest, bool SkipCompletionCheck)
{
	FQuestWrapper* QuestWrapper = Quests.Find(Quest.QuestAsset);
	if(!QuestWrapper)
	{
		return;
	}
	
	if(!SkipCompletionCheck)
	{
		if(!CanCompleteQuest(*QuestWrapper))
		{
			return;
		}
	}

	QuestWrapper->State = Finished;
	QuestStateUpdated.Broadcast(Quest, Finished);
		
#if ENABLE_VISUAL_LOG
		
	UE_VLOG_LOCATION(this, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(),
	10, FColor::White, TEXT("Completed quest: %s"), *Quest.QuestAsset->GetName());
		
#endif

	//Notify listeners
	for(const auto& CurrentListener : QuestWrapper->Listeners)
	{
		if(IsValid(CurrentListener))
		{
			if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
			{
				II_QuestUpdates::Execute_QuestStateUpdated(CurrentListener, *QuestWrapper, Finished);
			}
		}
	}

	//Quest is completed, no need to keep the listeners array active
	QuestWrapper->Listeners.Empty();
}

bool UQuestSubSystem::CanCompleteQuest_Implementation(FQuestWrapper Quest)
{
	if(!Quest.QuestAsset.IsValid())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Invalid Quest Asset for quest"), true, true);
		return false;
	}
	
	if(Quest.State != InProgress)
	{
		return false;
	}

	if(Quest.Tasks.IsEmpty())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Quest has no tasks, can't complete"), true, true);
		return false;
	}

	return true;
}

bool UQuestSubSystem::HasCompletedQuest(const TSoftObjectPtr<UDA_Quest>& Quest)
{
	if(FQuestWrapper* QuestWrapper = Quests.Find(Quest))
	{
		return QuestWrapper->State == Finished;
	}

	return false;
}

bool UQuestSubSystem::HasFailedQuest(const TSoftObjectPtr<UDA_Quest>& Quest)
{
	if(FQuestWrapper* QuestWrapper = Quests.Find(Quest))
	{
		return QuestWrapper->State == Failed;
	}

	return false;
}

TEnumAsByte<EQuestState> UQuestSubSystem::GetQuestState(TSoftObjectPtr<UDA_Quest> Quest)
{
	if(FQuestWrapper* QuestWrapper = Quests.Find(Quest))
	{
		return QuestWrapper->State;
	}

	return Inactive;
}

bool UQuestSubSystem::DropQuest(FQuestWrapper Quest)
{
	FQuestWrapper* QuestWrapper = Quests.Find(Quest.QuestAsset);
	if(!QuestWrapper)
	{
		return false;
	}
	
	//Drop the tasks
	for(const auto& CurrentTask : QuestWrapper->Tasks)
	{
		TaskDropped.Broadcast(CurrentTask);
			
		for(auto& CurrentListener : CurrentTask.Listeners)
		{
			if(IsValid(CurrentListener))
			{
				if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
				{
					II_QuestUpdates::Execute_TaskDropped(CurrentListener, CurrentTask);
				}
			}
		}
	}

	QuestDropped.Broadcast(Quest);

	//Announce the quest being dropped
	for(const auto& CurrentListener : QuestWrapper->Listeners)
	{
		if(IsValid(CurrentListener))
		{
			if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
			{
				II_QuestUpdates::Execute_QuestDropped(CurrentListener, Quest);
			}
		}
	}

	Quests.Remove(Quest.QuestAsset);

#if ENABLE_VISUAL_LOG
	UE_VLOG_LOCATION(this, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(),
	10, FColor::White, TEXT("Dropped quest: %s"),
	*Quest.QuestAsset->GetName());
#endif

	return true;
}

bool UQuestSubSystem::FailQuest(FQuestWrapper Quest, const bool FailTasks)
{
	FQuestWrapper* QuestWrapper = Quests.Find(Quest.QuestAsset);
	if(!QuestWrapper)
	{
		return false;
	}
	
	if(QuestWrapper->State != InProgress)
	{
		return false;
	}

	QuestWrapper->State = Failed;

#if ENABLE_VISUAL_LOG
	UE_VLOG_LOCATION(this, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(),
	10, FColor::White, TEXT("Failed quest: %s"),
	*Quest.QuestAsset->GetName());
#endif
		
	if(FailTasks)
	{
		//Fail the tasks
		for(auto& CurrentTask : QuestWrapper->Tasks)
		{
			CurrentTask.State = Failed;
			TaskFailed.Broadcast(CurrentTask);
			
			for(auto& CurrentListener : CurrentTask.Listeners)
			{
				if(IsValid(CurrentListener))
				{
					if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
					{
						II_QuestUpdates::Execute_TaskFailed(CurrentListener, CurrentTask);
					}
				}
			}
		}
	}

	QuestFailed.Broadcast(Quest);

	//Announce the quest failure
	for(const auto& CurrentListener : QuestWrapper->Listeners)
	{
		if(IsValid(CurrentListener))
		{
			if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
			{
				II_QuestUpdates::Execute_QuestFailed(CurrentListener, Quest);
			}
		}
	}

	return true;
}

TArray<FQuestWrapper> UQuestSubSystem::GetQuestsWithState(TEnumAsByte<EQuestState> State)
{
	TArray<FQuestWrapper> FoundQuests;

	for(auto& CurrentQuest : Quests)
	{
		if(CurrentQuest.Value.State == State)
		{
			FoundQuests.Add(CurrentQuest.Value);
		}
	}

	return FoundQuests;
}

FQuestWrapper UQuestSubSystem::GetQuestForTask(const FGameplayTag Task)
{
	for(auto& CurrentQuest : Quests)
	{
		for(auto& CurrentTask : CurrentQuest.Value.Tasks)
		{
			if(CurrentTask.TaskID == Task)
			{
				return CurrentQuest.Value;
			}
		}
	}

	return FQuestWrapper();
}

TArray<FTaskWrapper> UQuestSubSystem::GetTasksForQuest(const TSoftObjectPtr<UDA_Quest>& Quest)
{
	if(FQuestWrapper* QuestWrapper = Quests.Find(Quest))
	{
		return QuestWrapper->Tasks;
	}
	
	return TArray<FTaskWrapper>();
}

void UQuestSubSystem::AddListenerToTask(FGameplayTag Task, UObject* Listener)
{
	FQuestWrapper FoundQuest = GetQuestForTask(Task);
	if(!FoundQuest.QuestAsset)
	{
		return;
	}

	//Get a pointer to the real quest wrapper
	FQuestWrapper* QuestWrapper = Quests.Find(FoundQuest.QuestAsset);
	if(!QuestWrapper)
	{
		//This should NEVER happen
		return;
	}

	for(auto& CurrentTask : QuestWrapper->Tasks)
	{
		if(CurrentTask.TaskID == Task)
		{
			CurrentTask.Listeners.Add(Listener);
			return;
		}
	}
}

bool UQuestSubSystem::ProgressTask(const FGameplayTag Task, float ProgressToAdd, UObject* Instigator)
{
	FQuestWrapper FoundQuest = GetQuestForTask(Task);
	if(!FoundQuest.QuestAsset)
	{
		return false;
	}

	//Get a pointer to the real quest wrapper
	FQuestWrapper* QuestWrapper = Quests.Find(FoundQuest.QuestAsset);
	if(!QuestWrapper)
	{
		//This should NEVER happen
		return false;
	}

	bool TaskCompleted = false;
	bool QuestCompleted = true;
	
	for(auto& CurrentTask : QuestWrapper->Tasks)
	{
		if(CurrentTask.TaskID == Task)
		{
			//Only tasks that are in progress can be progressed.
			if(CurrentTask.State != InProgress)
			{
				return false;
			}

			//Progress is less than 0, don't bother with math and delegate.
			if(CurrentTask.CurrentProgress < 0)
			{
				return false;
			}

			if(!CanTaskBeProgressed(CurrentTask.TaskID))
			{
				return false;
			}

			const float ProgressRequired = CurrentTask.RootQuest->GetRequiredTaskProgression(CurrentTask.TaskID);
			const float ProgressDelta = (UKismetMathLibrary::Clamp(CurrentTask.CurrentProgress + ProgressToAdd, 0,ProgressRequired) - CurrentTask.CurrentProgress);
			
			CurrentTask.CurrentProgress = UKismetMathLibrary::Clamp(CurrentTask.CurrentProgress + ProgressToAdd,0,ProgressRequired);
			
			if(CurrentTask.CurrentProgress == ProgressRequired)
			{
				CurrentTask.State = Finished;
				TaskCompleted = true;
			}
			TaskProgressed.Broadcast(CurrentTask, ProgressDelta, Instigator);
			
			//Notify listeners
			for(const auto& CurrentListener : CurrentTask.Listeners)
			{
				if(IsValid(CurrentListener))
				{
					if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
					{
						II_QuestUpdates::Execute_TaskProgressed(CurrentListener, CurrentTask, ProgressDelta, Instigator);
					}
				}
			}
			
			#if ENABLE_VISUAL_LOG
			UE_VLOG_LOCATION(this, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(),
			10, FColor::White, TEXT("Progressed task %s - %s / %s"),
			*Task.ToString(),
			*FString::SanitizeFloat(CurrentTask.CurrentProgress),
			*FString::SanitizeFloat(ProgressRequired));
			#endif
		}

		if(CurrentTask.State == InProgress && CurrentTask.RootQuest->IsTaskOptional(CurrentTask.TaskID) != true)
		{
			QuestCompleted = false;
		}
		
	}

	if(QuestCompleted)
	{
		CompleteQuest(*QuestWrapper, false);
	}

	if(TaskCompleted)
	{
		return true;
	}
	
	return false;
}

bool UQuestSubSystem::CanTaskBeProgressed(FGameplayTag Task)
{
	FQuestWrapper FoundQuest = GetQuestForTask(Task);
	if(!FoundQuest.QuestAsset)
	{
		return false;
	}

	//Get a pointer to the real quest wrapper
	FQuestWrapper* QuestWrapper = Quests.Find(FoundQuest.QuestAsset);
	if(!QuestWrapper)
	{
		//This should NEVER happen
		return false;
	}

	for(auto& CurrentTask : QuestWrapper->Tasks)
	{
		if(CurrentTask.TaskID == Task)
		{
			if(CurrentTask.State != InProgress)
			{
				return false;
			}
			
			if(CurrentTask.CurrentProgress >= CurrentTask.RootQuest->GetRequiredTaskProgression(CurrentTask.TaskID))
			{
				return false;
			}

			if(UFL_QuestHelpers::IsTaskRequirementsMet(this, CurrentTask.RootQuest->GetTasksRequirements(CurrentTask.TaskID)))
			{
				
			}
			
			if(Execute_PreventTaskProgress(this, CurrentTask))
			{
				return false;
			}

			for(const auto& CurrentListener : CurrentTask.Listeners)
			{
				if(IsValid(CurrentListener))
				{
					if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
					{
						if(Execute_PreventTaskProgress(CurrentListener, CurrentTask))
						{
							return false;
						}
					}
				}
			}
		}
	}
	
	return true;
}

bool UQuestSubSystem::FailTask(const FGameplayTag Task, const bool bFailQuest)
{
	FQuestWrapper FoundQuest = GetQuestForTask(Task);
	if(!FoundQuest.QuestAsset)
	{
		return false;
	}

	//Get a pointer to the real quest wrapper
	FQuestWrapper* QuestWrapper = Quests.Find(FoundQuest.QuestAsset);
	if(!QuestWrapper)
	{
		//This should NEVER happen
		return false;
	}

	for(auto& CurrentTask : QuestWrapper->Tasks)
	{
		if(CurrentTask.TaskID == Task)
		{
			CurrentTask.State = Failed;

			#if ENABLE_VISUAL_LOG
			UE_VLOG_LOCATION(this, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(),
			10, FColor::White, TEXT("Failed task: %s"),
			*Task.ToString());
			#endif

			TaskFailed.Broadcast(CurrentTask);
				
			for(auto& CurrentListener : CurrentTask.Listeners)
			{
				if(IsValid(CurrentListener))
				{
					if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
					{
						II_QuestUpdates::Execute_TaskFailed(CurrentListener, CurrentTask);
					}
				}
			}
			break;
		}
	}
		
	if(bFailQuest)
	{
		//Since we are failing a specific task, don't go ahead and fail the others.
		FailQuest(*QuestWrapper, false);
	}

	return true;
}

bool UQuestSubSystem::AddTaskToQuest(FQuestTask Task, TSoftObjectPtr<UDA_Quest> Quest)
{
	// const int32 QuestIndex = GetQuestIndex_Active(Quest);
	//
	// if(ActiveQuests.IsValidIndex(QuestIndex))
	// {
	// 	const FTaskWrapper TaskWrapper = UFL_QuestHelpers::WrapTask(Quest.LoadSynchronous(), Task);
	//
	// 	ActiveQuests[QuestIndex].Tasks.Add(TaskWrapper);
	//
	// 	TaskAddedToQuest.Broadcast(TaskWrapper, ActiveQuests[QuestIndex]);
	//
	// 	for(const auto& CurrentListener : ActiveQuests[QuestIndex].Listeners)
	// 	{
	// 		if(IsValid(CurrentListener))
	// 		{
	// 			if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
	// 			{
	// 				II_QuestUpdates::Execute_TaskAddedToQuest(CurrentListener, TaskWrapper, ActiveQuests[QuestIndex]);
	// 			}
	// 		}
	// 	}
	// 	
	// 	return true;
	// }

	return false;
}

bool UQuestSubSystem::RemoveTaskFromQuest(FGameplayTag Task, TSoftObjectPtr<UDA_Quest> Quest)
{
	// const int32 QuestIndex = GetQuestIndex_Active(Quest);
	//
	// if(ActiveQuests.IsValidIndex(QuestIndex))
	// {
	// 	FQuestWrapper& QuestWrapper = ActiveQuests[QuestIndex];
	// 	for(int32 CurrentIndex = 0; CurrentIndex < QuestWrapper.Tasks.Num(); CurrentIndex++)
	// 	{
	// 		if(QuestWrapper.Tasks[CurrentIndex].TaskID == Task)
	// 		{
	// 			const FTaskWrapper TaskWrapper = QuestWrapper.Tasks[CurrentIndex];
	// 			QuestWrapper.Tasks.RemoveAt(CurrentIndex);
	//
	// 			TaskRemovedFromQuest.Broadcast(TaskWrapper, ActiveQuests[QuestIndex]);
	//
	// 			for(const auto& CurrentListener : ActiveQuests[QuestIndex].Listeners)
	// 			{
	// 				if(IsValid(CurrentListener))
	// 				{
	// 					if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
	// 					{
	// 						II_QuestUpdates::Execute_TaskRemovedFromQuest(CurrentListener, TaskWrapper, ActiveQuests[QuestIndex]);
	// 					}
	// 				}
	// 			}
	//
	// 			for(const auto& CurrentListener : TaskWrapper.Listeners)
	// 			{
	// 				if(IsValid(CurrentListener))
	// 				{
	// 					if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
	// 					{
	// 						II_QuestUpdates::Execute_TaskRemovedFromQuest(CurrentListener, TaskWrapper, ActiveQuests[QuestIndex]);
	// 					}
	// 				}
	// 			}
	// 			break;
	// 		}
	// 	}
	//
	// 	#if ENABLE_VISUAL_LOG
	// 	UE_VLOG_LOCATION(this, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(),
	// 	10, FColor::White, TEXT("Removed task (%s) from quest (%s)"),
	// 	*Task.ToString(),
	// 	*Quest->GetName());
	// 	#endif
	//
	// 	//If all the remaining tasks are optional, drop the quest.
	// 	bool RemainingTasksAreOptional = true;
	// 	for(auto& CurrentTask : QuestWrapper.Tasks)
	// 	{
	// 		if(!CurrentTask.RootQuest->IsTaskOptional(CurrentTask.TaskID))
	// 		{
	// 			RemainingTasksAreOptional = false;
	// 			break;
	// 		}
	// 	}
	//
	// 	if(QuestWrapper.Tasks.Num() == 0 || RemainingTasksAreOptional)
	// 	{
	// 		DropQuest(QuestWrapper);
	// 	}
	// 	
	// 	return true;
	// }

	return false;
}
