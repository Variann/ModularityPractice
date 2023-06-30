// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Quest/FL_QuestHelpers.h"
#include "Quest/I_QuestUpdates.h"


void UQuestComponent::AddListenerToQuest(FGameplayTag Quest, UObject* Listener)
{
	int32 QuestIndex = GetQuestIndex_Active(Quest);

	if(!ActiveQuests.IsValidIndex(QuestIndex))
	{
		return;
	}

	ActiveQuests[QuestIndex].Listeners.AddUnique(Listener);
}

FS_QuestWrapper UQuestComponent::GetQuestWrapper_Active(FGameplayTag Quest, int32& ArrayIndex)
{
	ArrayIndex = GetQuestIndex_Active(Quest);

	if(ActiveQuests.IsValidIndex(ArrayIndex))
	{
		return ActiveQuests[ArrayIndex];
	}

	return FS_QuestWrapper();
}

bool UQuestComponent::AcceptQuest(UFN_QuestBase* Quest)
{
	if(!IsValid(Quest))
	{
		return false;
	}
	
	if(!CanAcceptQuest(Quest->QuestInformation.QuestID))
	{
		return false;
	}

	//Player can accept the quest, start accepting it.

	//Wrap the quest into a struct that is more easily
	//serialized and manageable.
	FS_QuestWrapper QuestWrapper = UFL_QuestHelpers::WrapQuest(Quest->QuestInformation, Quest);

	//Wrap the tasks into a struct that is more easily
	//serialized and manageable.
	for(auto& CurrentTask : Quest->QuestInformation.Tasks)
	{
		FS_TaskWrapper TaskWrapper = UFL_QuestHelpers::WrapTask(CurrentTask);
		
		QuestWrapper.Tasks.Add(TaskWrapper);
	}

	ActiveQuests.Add(QuestWrapper);
	QuestStateUpdated.Broadcast(QuestWrapper, InProgress);
	
	return true;
}

bool UQuestComponent::CanAcceptQuest_Implementation(FGameplayTag Quest)
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

int32 UQuestComponent::GetQuestIndex_Active(FGameplayTag Quest)
{
	for(int32 CurrentIndex = 0; CurrentIndex < ActiveQuests.Num(); CurrentIndex++)
	{
		if(ActiveQuests[CurrentIndex].QuestID == Quest)
		{
			return CurrentIndex;
		}
	}

	return -1;
}

void UQuestComponent::CompleteQuest(FS_QuestWrapper Quest, bool SkipCompletionCheck)
{
	const int32 QuestIndex = GetQuestIndex_Active(Quest.QuestID);

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
		
		CompletedQuests[NewQuestIndex].Graph = nullptr;
		CompletedQuests[NewQuestIndex].ParentNode = nullptr;
		CompletedQuests[NewQuestIndex].Listeners.Empty();
	}
}

bool UQuestComponent::CanCompleteQuest_Implementation(FS_QuestWrapper Quest)
{
	if(!Quest.QuestID.IsValid())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Invalid QuestID for quest"), true, true);
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

bool UQuestComponent::HasCompletedQuest(FGameplayTag Quest)
{
	for(auto& CurrentQuest : CompletedQuests)
	{
		if(CurrentQuest.QuestID == Quest)
		{
			return true;
		}
	}

	return false;
}

bool UQuestComponent::IsQuestActive(FGameplayTag Quest)
{
	for(auto& CurrentQuest : ActiveQuests)
	{
		if(CurrentQuest.QuestID == Quest)
		{
			return true;
		}
	}

	return false;
}

bool UQuestComponent::HasFailedQuest(FGameplayTag Quest)
{
	for(auto& CurrentQuest : FailedQuests)
	{
		if(CurrentQuest.QuestID == Quest)
		{
			return true;
		}
	}

	return false;
}

bool UQuestComponent::DropQuest(FS_QuestWrapper Quest)
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

		return true;
	}

	return false;
}

bool UQuestComponent::FailQuest(FS_QuestWrapper Quest, bool FailTasks)
{
	if(Quest.State != InProgress)
	{
		return false;
	}

	Quest.State = Failed;
	ActiveQuests.RemoveSingle(Quest);
	FailedQuests.Add(Quest);

	if(ActiveQuests.Contains(Quest))
	{
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

FS_QuestWrapper UQuestComponent::GetQuestForTask_Active(FGameplayTag Task, int32& ArrayIndex)
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

	return FS_QuestWrapper();
}

TArray<FS_TaskWrapper> UQuestComponent::GetTasksForQuest_Active(FGameplayTag Quest)
{
	TArray<FS_TaskWrapper> FoundTasks;
	const int32 QuestIndex = GetQuestIndex_Active(Quest);

	if(ActiveQuests.IsValidIndex(QuestIndex))
	{
		return ActiveQuests[QuestIndex].Tasks;
	}
	
	return FoundTasks;
}

void UQuestComponent::AddListenerToTask(FGameplayTag Task, UObject* Listener)
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

bool UQuestComponent::ProgressTask(const FGameplayTag Task, float ProgressToAdd, UObject* Instigator)
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

			//Progress is less than 0, don't bother with math ahd delegate.
			if(CurrentTask.CurrentProgress < 0)
			{
				return false;
			}

			if(!CanTaskBeProgressed(CurrentTask))
			{
				return false;
			}
			
			const float ProgressDelta = (UKismetMathLibrary::Clamp(CurrentTask.CurrentProgress + ProgressToAdd, 0,CurrentTask.ProgressRequired) - CurrentTask.CurrentProgress);
			
			CurrentTask.CurrentProgress = UKismetMathLibrary::Clamp(CurrentTask.CurrentProgress + ProgressToAdd,0,CurrentTask.ProgressRequired);

			if(CurrentTask.CurrentProgress == CurrentTask.ProgressRequired)
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
		}

		if(CurrentTask.State == InProgress && CurrentTask.IsOptional != true)
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

bool UQuestComponent::CanTaskBeProgressed(FS_TaskWrapper Task)
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
			if(CurrentTask.CurrentProgress >= CurrentTask.ProgressRequired)
			{
				return false;
			}
			
			if(CurrentTask.State == InProgress)
			{
				bool ProgressTask = true;
				if(II_QuestUpdates::Execute_PreventTaskProgress(this, CurrentTask))
				{
					return false;
				}

				for(const auto& CurrentListener : CurrentTask.Listeners)
				{
					if(IsValid(CurrentListener))
					{
						if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
						{
							if(II_QuestUpdates::Execute_PreventTaskProgress(CurrentListener, CurrentTask))
							{
								return false;
							}
						}
					}
				}
			}
		}
	}
	
	return false;
}

bool UQuestComponent::FailTask(FGameplayTag Task, bool FailQuest)
{
	int32 QuestIndex;
	GetQuestForTask_Active(Task, QuestIndex);

	if(ActiveQuests.IsValidIndex(QuestIndex))
	{
		FS_QuestWrapper& Quest = ActiveQuests[QuestIndex];

		for(auto& CurrentTask : Quest.Tasks)
		{
			if(CurrentTask.TaskID == Task)
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
				break;
			}
		}
		
		if(FailQuest)
		{
			Quest.State = Failed;

			QuestFailed.Broadcast(Quest);

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
		}

		return true;
	}
	
	return false;
}

bool UQuestComponent::AddTaskToQuest(FS_QuestTask Task, FGameplayTag Quest)
{
	const int32 QuestIndex = GetQuestIndex_Active(Quest);

	if(ActiveQuests.IsValidIndex(QuestIndex))
	{
		const FS_TaskWrapper TaskWrapper = UFL_QuestHelpers::WrapTask(Task);

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

bool UQuestComponent::RemoveTaskFromQuest(FGameplayTag Task, FGameplayTag Quest)
{
	const int32 QuestIndex = GetQuestIndex_Active(Quest);

	if(ActiveQuests.IsValidIndex(QuestIndex))
	{
		FS_QuestWrapper& QuestWrapper = ActiveQuests[QuestIndex];
		for(int32 CurrentIndex = 0; CurrentIndex < QuestWrapper.Tasks.Num(); CurrentIndex++)
		{
			if(QuestWrapper.Tasks[CurrentIndex].TaskID == Task)
			{
				const FS_TaskWrapper TaskWrapper = QuestWrapper.Tasks[CurrentIndex];
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

				if(QuestWrapper.Tasks.Num() == 0)
				{
					DropQuest(QuestWrapper);
				}
			}
		}
		return true;
	}

	return false;
}

bool UQuestComponent::CanTaskBeProgressed_Internal_Implementation(FS_TaskWrapper Task)
{
	return true;
}
