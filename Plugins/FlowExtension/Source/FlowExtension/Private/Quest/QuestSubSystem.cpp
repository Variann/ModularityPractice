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
	const int32 QuestIndex = GetQuestIndex_Active(Quest);

	if(!ActiveQuests.IsValidIndex(QuestIndex))
	{
		return;
	}

	ActiveQuests[QuestIndex].Listeners.AddUnique(Listener);
}

FQuestWrapper UQuestSubSystem::GetQuestWrapper_Active(TSoftObjectPtr<UDA_Quest> Quest, int32& ArrayIndex)
{
	ArrayIndex = GetQuestIndex_Active(Quest);

	if(ActiveQuests.IsValidIndex(ArrayIndex))
	{
		return ActiveQuests[ArrayIndex];
	}

	return FQuestWrapper();
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

	ActiveQuests.Add(QuestWrapper);
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

	if(IsQuestActive(Quest))
	{
		return false;
	}

	if(HasFailedQuest(Quest))
	{
		return false;
	}

	return true;
}

int32 UQuestSubSystem::GetQuestIndex_Active(const TSoftObjectPtr<UDA_Quest>& Quest)
{
	for(int32 CurrentIndex = 0; CurrentIndex < ActiveQuests.Num(); CurrentIndex++)
	{
		if(ActiveQuests[CurrentIndex].QuestAsset == Quest)
		{
			return CurrentIndex;
		}
	}

	return -1;
}

void UQuestSubSystem::CompleteQuest(FQuestWrapper Quest, bool SkipCompletionCheck)
{
	const int32 QuestIndex = GetQuestIndex_Active(Quest.QuestAsset);

	if(ActiveQuests.IsValidIndex(QuestIndex))
	{
		if(!SkipCompletionCheck)
		{
			if(!CanCompleteQuest(ActiveQuests[QuestIndex]))
			{
				return;
			}
		}

		ActiveQuests[QuestIndex].State = Finished;

		//Move the quest to the completed array.
		int32 NewQuestIndex = CompletedQuests.Add(ActiveQuests[QuestIndex]);
		ActiveQuests.RemoveAt(QuestIndex);

		QuestStateUpdated.Broadcast(CompletedQuests[NewQuestIndex], Finished);
		
		#if ENABLE_VISUAL_LOG
		
		UE_VLOG_LOCATION(this, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(),
		10, FColor::White, TEXT("Completed quest: %s"), *Quest.QuestAsset->GetName());
		
		#endif

		//Notify listeners
		for(const auto& CurrentListener : CompletedQuests[NewQuestIndex].Listeners)
		{
			if(IsValid(CurrentListener))
			{
				if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
				{
					II_QuestUpdates::Execute_QuestStateUpdated(CurrentListener, CompletedQuests[NewQuestIndex], Finished);
				}
			}
		}
		
		CompletedQuests[NewQuestIndex].Listeners.Empty();
	}
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
	for(auto& CurrentQuest : CompletedQuests)
	{
		if(CurrentQuest.QuestAsset == Quest)
		{
			return true;
		}
	}

	return false;
}

bool UQuestSubSystem::IsQuestActive(const TSoftObjectPtr<UDA_Quest>& Quest)
{
	for(auto& CurrentQuest : ActiveQuests)
	{
		if(CurrentQuest.QuestAsset == Quest)
		{
			return true;
		}
	}

	return false;
}

bool UQuestSubSystem::HasFailedQuest(const TSoftObjectPtr<UDA_Quest>& Quest)
{
	for(auto& CurrentQuest : FailedQuests)
	{
		if(CurrentQuest.QuestAsset == Quest)
		{
			return true;
		}
	}

	return false;
}

TEnumAsByte<EQuestState> UQuestSubSystem::GetQuestState(TSoftObjectPtr<UDA_Quest> Quest)
{
	for(auto& CurrentQuest : ActiveQuests)
	{
		if(CurrentQuest.QuestAsset == Quest)
		{
			return CurrentQuest.State;
		}
	}
	
	for(auto& CurrentQuest : FailedQuests)
	{
		if(CurrentQuest.QuestAsset == Quest)
		{
			return CurrentQuest.State;
		}
	}

	for(auto& CurrentQuest : CompletedQuests)
	{
		if(CurrentQuest.QuestAsset == Quest)
		{
			return CurrentQuest.State;
		}
	}

	return Inactive;
}

bool UQuestSubSystem::DropQuest(FQuestWrapper Quest)
{
	if(Quest.State != InProgress)
	{
		return false;
	}

	if(ActiveQuests.Contains(Quest))
	{
		//Drop the tasks
		for(const auto& CurrentTask : Quest.Tasks)
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
		for(const auto& CurrentListener : Quest.Listeners)
		{
			if(IsValid(CurrentListener))
			{
				if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
				{
					II_QuestUpdates::Execute_QuestDropped(CurrentListener, Quest);
				}
			}
		}

		ActiveQuests.RemoveSingle(Quest);

		#if ENABLE_VISUAL_LOG
		UE_VLOG_LOCATION(this, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(),
		10, FColor::White, TEXT("Dropped quest: %s"),
		*Quest.QuestAsset->GetName());
		#endif

		return true;
	}

	return false;
}

bool UQuestSubSystem::FailQuest(FQuestWrapper Quest, const bool FailTasks)
{
	if(Quest.State != InProgress)
	{
		return false;
	}

	if(ActiveQuests.Contains(Quest))
	{
		Quest.State = Failed;
		ActiveQuests.RemoveSingle(Quest);
		FailedQuests.Add(Quest);

#if ENABLE_VISUAL_LOG
		UE_VLOG_LOCATION(this, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(),
	10, FColor::White, TEXT("Failed quest: %s"),
	*Quest.QuestAsset->GetName());
#endif
		
		if(FailTasks)
		{
			//Fail the tasks
			for(auto& CurrentTask : Quest.Tasks)
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
		for(const auto& CurrentListener : Quest.Listeners)
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

	return false;
}

FQuestWrapper UQuestSubSystem::GetQuestForTask_Active(const FGameplayTag Task, int32& ArrayIndex)
{
	ArrayIndex = -1;
	for(int32 CurrentIndex = 0; CurrentIndex < ActiveQuests.Num(); CurrentIndex++)
	{
		for(auto& CurrentTask : ActiveQuests[CurrentIndex].Tasks)
		{
			if(CurrentTask.TaskID == Task)
			{
				ArrayIndex = CurrentIndex;
				return ActiveQuests[CurrentIndex];
			}
		}
	}

	return FQuestWrapper();
}

TArray<FTaskWrapper> UQuestSubSystem::GetTasksForQuest_Active(const TSoftObjectPtr<UDA_Quest>& Quest)
{
	TArray<FTaskWrapper> FoundTasks;
	const int32 QuestIndex = GetQuestIndex_Active(Quest);

	if(ActiveQuests.IsValidIndex(QuestIndex))
	{
		return ActiveQuests[QuestIndex].Tasks;
	}
	
	return FoundTasks;
}

void UQuestSubSystem::AddListenerToTask(FGameplayTag Task, UObject* Listener)
{
	int32 QuestIndex = 0;
	GetQuestForTask_Active(Task, QuestIndex);

	if(ActiveQuests.IsValidIndex(QuestIndex))
	{
		for(auto& CurrentTask : ActiveQuests[QuestIndex].Tasks)
		{
			if(CurrentTask.TaskID == Task)
			{
				CurrentTask.Listeners.AddUnique(Listener);
			}
		}
	}
}

bool UQuestSubSystem::ProgressTask(const FGameplayTag Task, float ProgressToAdd, UObject* Instigator)
{
	int32 QuestIndex;
	GetQuestForTask_Active(Task, QuestIndex);

	bool TaskCompleted = false;
	bool QuestCompleted = true;

	if(!ActiveQuests.IsValidIndex(QuestIndex))
	{
		return false;
	}
	
	for(auto& CurrentTask : ActiveQuests[QuestIndex].Tasks)
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

			if(!CanTaskBeProgressed(CurrentTask))
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
		CompleteQuest(ActiveQuests[QuestIndex], false);
	}

	if(TaskCompleted)
	{
		return true;
	}
	
	return false;
}

bool UQuestSubSystem::CanTaskBeProgressed(FTaskWrapper Task)
{
	int32 QuestIndex;
	GetQuestForTask_Active(Task.TaskID, QuestIndex);

	if(!ActiveQuests.IsValidIndex(QuestIndex))
	{
		return false;
	}

	for(auto& CurrentTask : ActiveQuests[QuestIndex].Tasks)
	{
		if(CurrentTask.TaskID == Task.TaskID)
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
	int32 QuestIndex;
	GetQuestForTask_Active(Task, QuestIndex);

	if(ActiveQuests.IsValidIndex(QuestIndex))
	{
		FQuestWrapper& Quest = ActiveQuests[QuestIndex];

		for(auto& CurrentTask : Quest.Tasks)
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
			FailQuest(Quest, false);
		}

		return true;
	}
	
	return false;
}

bool UQuestSubSystem::AddTaskToQuest(FQuestTask Task, TSoftObjectPtr<UDA_Quest> Quest)
{
	const int32 QuestIndex = GetQuestIndex_Active(Quest);

	if(ActiveQuests.IsValidIndex(QuestIndex))
	{
		const FTaskWrapper TaskWrapper = UFL_QuestHelpers::WrapTask(Quest.LoadSynchronous(), Task);

		ActiveQuests[QuestIndex].Tasks.Add(TaskWrapper);

		TaskAddedToQuest.Broadcast(TaskWrapper, ActiveQuests[QuestIndex]);

		for(const auto& CurrentListener : ActiveQuests[QuestIndex].Listeners)
		{
			if(IsValid(CurrentListener))
			{
				if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
				{
					II_QuestUpdates::Execute_TaskAddedToQuest(CurrentListener, TaskWrapper, ActiveQuests[QuestIndex]);
				}
			}
		}
		
		return true;
	}

	return false;
}

bool UQuestSubSystem::RemoveTaskFromQuest(FGameplayTag Task, TSoftObjectPtr<UDA_Quest> Quest)
{
	const int32 QuestIndex = GetQuestIndex_Active(Quest);

	if(ActiveQuests.IsValidIndex(QuestIndex))
	{
		FQuestWrapper& QuestWrapper = ActiveQuests[QuestIndex];
		for(int32 CurrentIndex = 0; CurrentIndex < QuestWrapper.Tasks.Num(); CurrentIndex++)
		{
			if(QuestWrapper.Tasks[CurrentIndex].TaskID == Task)
			{
				const FTaskWrapper TaskWrapper = QuestWrapper.Tasks[CurrentIndex];
				QuestWrapper.Tasks.RemoveAt(CurrentIndex);

				TaskRemovedFromQuest.Broadcast(TaskWrapper, ActiveQuests[QuestIndex]);

				for(const auto& CurrentListener : ActiveQuests[QuestIndex].Listeners)
				{
					if(IsValid(CurrentListener))
					{
						if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
						{
							II_QuestUpdates::Execute_TaskRemovedFromQuest(CurrentListener, TaskWrapper, ActiveQuests[QuestIndex]);
						}
					}
				}

				for(const auto& CurrentListener : TaskWrapper.Listeners)
				{
					if(IsValid(CurrentListener))
					{
						if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
						{
							II_QuestUpdates::Execute_TaskRemovedFromQuest(CurrentListener, TaskWrapper, ActiveQuests[QuestIndex]);
						}
					}
				}
				break;
			}
		}

		#if ENABLE_VISUAL_LOG
		UE_VLOG_LOCATION(this, TEXT("Quest System %s"), Verbose, UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation(),
		10, FColor::White, TEXT("Removed task (%s) from quest (%s)"),
		*Task.ToString(),
		*Quest->GetName());
		#endif

		//If all the remaining tasks are optional, drop the quest.
		bool RemainingTasksAreOptional = true;
		for(auto& CurrentTask : QuestWrapper.Tasks)
		{
			if(!CurrentTask.RootQuest->IsTaskOptional(CurrentTask.TaskID))
			{
				RemainingTasksAreOptional = false;
				break;
			}
		}

		if(QuestWrapper.Tasks.Num() == 0 || RemainingTasksAreOptional)
		{
			DropQuest(QuestWrapper);
		}
		
		return true;
	}

	return false;
}
