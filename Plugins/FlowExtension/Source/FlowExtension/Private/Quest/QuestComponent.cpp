// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestComponent.h"

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
