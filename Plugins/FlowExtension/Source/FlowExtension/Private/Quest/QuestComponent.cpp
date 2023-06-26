﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestComponent.h"

bool UQuestComponent::AcceptQuest(UFN_QuestBase* Quest)
{
	if(!CanAcceptQuest(Quest->QuestInformation.QuestID))
	{
		return false;
	}

	//Player can accept the quest, start accepting it.

	FS_QuestWrapper QuestWrapper;
	QuestWrapper.Graph = Quest->GetFlowAsset();
	QuestWrapper.ParentNode = Quest;
	
	return false;
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
