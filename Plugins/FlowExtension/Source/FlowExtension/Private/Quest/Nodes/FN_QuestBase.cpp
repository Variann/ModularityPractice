// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/Nodes/FN_QuestBase.h"

#include "Kismet/KismetStringLibrary.h"

UFN_QuestBase::UFN_QuestBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Quest");
#endif

	OutputPins = {FFlowPin(TEXT("Out")), FFlowPin(TEXT("Accepted")), FFlowPin(TEXT("Completed"))
	, FFlowPin(TEXT("Failed")), FFlowPin(TEXT("Abandoned")), FFlowPin(TEXT("Failed Requirements"))};
}

#if WITH_EDITOR
FString UFN_QuestBase::GetNodeDescription() const
{
	if(!QuestAsset.IsValid())
	{
		return Super::GetNodeDescription();
	}

	//Load and retrieve the quest ID
	if(const FGameplayTag QuestID = QuestAsset.LoadSynchronous()->QuestID; QuestID.IsValid())
	{
		FString ShortenedString;
		FString LeftString;
		UKismetStringLibrary::Split(QuestID.ToString(), "Quests.", LeftString, ShortenedString, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		return ShortenedString;
	}
	
	return Super::GetNodeDescription();
}

EDataValidationResult UFN_QuestBase::ValidateNode()
{
	bool FailedValidation = false;
	QuestPtr = QuestAsset.LoadSynchronous();

	if(!QuestAsset.IsValid())
	{
		ValidationLog.Error<UFlowNode>(TEXT("Missing Quest Asset"), this);
		FailedValidation = true;
	}
	
	if(!QuestPtr->QuestID.IsValid())
	{
		ValidationLog.Error<UFlowNode>(TEXT("Missing QuestID"), this);
		FailedValidation = true;
	}

	if(QuestPtr->Tasks.IsEmpty())
	{
		ValidationLog.Error<UFlowNode>(TEXT("Quest has no tasks"), this);
		FailedValidation = true;
	}
	
	for(auto& CurrentTask : QuestPtr->Tasks)
	{
		if(CurrentTask.ProgressRequired <= 0)
		{
			ValidationLog.Error<UFlowNode>(TEXT("Progress for task too low"), this);
			FailedValidation = true;
		}
	}

	if(QuestPtr->QuestText.IsEmpty())
	{
		ValidationLog.Error<UFlowNode>(TEXT("No quest text"), this);
		FailedValidation = true;
	}

	if(FailedValidation)
	{
		return EDataValidationResult::Invalid;
	}

	/**Remember to wipe the reference so it doesn't permanently
	 * stay in memory.*/
	QuestPtr = nullptr;
	
	return EDataValidationResult::Valid;
}
#endif
