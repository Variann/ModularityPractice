// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Quest/I_QuestUpdates.h"

bool UQuestComponent::AcceptQuest(UFN_QuestBase* Quest)
{
	if(!CanAcceptQuest(Quest->QuestInformation.QuestID))
	{
		return false;
	}

	//Player can accept the quest, start accepting it.

	//Wrap the quest into a struct that is more easily
	//serialized and manageable.
	FS_QuestWrapper QuestWrapper;
	QuestWrapper.Graph = Quest->GetFlowAsset();
	QuestWrapper.ParentNode = Quest;
	QuestWrapper.QuestID = Quest->QuestInformation.QuestID;
	QuestWrapper.State = InProgress;

	//Wrap the tasks into a struct that is more easily
	//serialized and manageable.
	for(auto& CurrentTask : Quest->QuestInformation.Tasks)
	{
		FS_TaskWrapper TaskWrapper;
		TaskWrapper.TaskID = CurrentTask.TaskID;
		TaskWrapper.Requirements = CurrentTask.Requirements;
		TaskWrapper.FailConditions = CurrentTask.FailConditions;
		TaskWrapper.State = InProgress;
		TaskWrapper.ProgressRequired = CurrentTask.ProgressRequired;
		
		QuestWrapper.Tasks.Add(TaskWrapper);
	}

	ActiveQuests.Add(QuestWrapper);
	QuestStateUpdated.Broadcast(QuestWrapper, InProgress);
	
	return true;
}

bool UQuestComponent::CanAcceptQuest(FGameplayTag Quest)
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

void UQuestComponent::CompleteQuest(FGameplayTag Quest)
{
	const int32 QuestIndex = GetQuestIndex_Active(Quest);

	if(ActiveQuests.IsValidIndex(QuestIndex))
	{
		FS_QuestWrapper& QuestWrapper = ActiveQuests[QuestIndex];
		QuestWrapper.State = Finished;
		QuestWrapper.Listeners.Empty();
		QuestWrapper.Graph = nullptr;
		QuestWrapper.ParentNode = nullptr;

		QuestStateUpdated.Broadcast(QuestWrapper, Finished);

		//Notify listeners
		for(const auto& CurrentListener : QuestWrapper.Listeners)
		{
			if(IsValid(CurrentListener))
			{
				if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_QuestUpdates::StaticClass()))
				{
					II_QuestUpdates::Execute_QuestStateUpdated(CurrentListener, QuestWrapper, Finished);
				}
			}
		}
	}
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

TMap<FGameplayTag, bool> UQuestComponent::HasCompletedQuests(FGameplayTagContainer Quests)
{
	TMap<FGameplayTag, bool> FoundQuests;
	
	for(auto& CurrentQuest : CompletedQuests)
	{
		if(Quests.HasTagExact(CurrentQuest.QuestID))
		{
			FoundQuests.Add(CurrentQuest.QuestID, true);
		}
	}

	for(auto& CurrentFound : FoundQuests)
	{
		if(!Quests.HasTagExact(CurrentFound.Key))
		{
			FoundQuests.Add(CurrentFound.Key, false);
		}
	}
	
	return FoundQuests;
	
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

			//Progress is 0 or less, don't bother with math ahd delegate.
			if(CurrentTask.CurrentProgress <= 0)
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

			continue;
		}

		if(CurrentTask.State == InProgress)
		{
			QuestCompleted = false;
		}
	}

	if(QuestCompleted)
	{
		CompleteQuest(ActiveQuests[QuestIndex].QuestID);
	}

	if(TaskCompleted)
	{
		return true;
	}
	
	return false;
}

bool UQuestComponent::CanTaskBeProgressed(const FGameplayTag Task)
{
	int32 QuestIndex;
	GetQuestForTask_Active(Task, QuestIndex);

	if(!ActiveQuests.IsValidIndex(QuestIndex))
	{
		return false;
	}

	for(auto& CurrentTask : ActiveQuests[QuestIndex].Tasks)
	{
		if(CurrentTask.TaskID == Task)
		{
			if(CurrentTask.CurrentProgress >= CurrentTask.ProgressRequired)
			{
				return false;
			}
			
			if(CurrentTask.State == InProgress)
			{
				return CanTaskBeProgressed_Internal(CurrentTask);
			}
		}
	}
	
	return false;
}

bool UQuestComponent::CanTaskBeProgressed_Internal_Implementation(FS_TaskWrapper Task)
{
	return true;
}
