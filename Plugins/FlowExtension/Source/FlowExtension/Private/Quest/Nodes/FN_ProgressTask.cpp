// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Quest/Nodes/FN_ProgressTask.h"

#include "Kismet/KismetStringLibrary.h"

UFN_ProgressTask::UFN_ProgressTask(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Quest");
#endif
}

#if WITH_EDITOR
FString UFN_ProgressTask::GetNodeDescription() const
{
	if(Task.IsValid())
	{
		FString ShortenedString;
		FString LeftString;
		UKismetStringLibrary::Split(Task.ToString(), "Quests.", LeftString, ShortenedString, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		return ShortenedString + "  -  " + FString::SanitizeFloat(ProgressToAdd);
	}
	
	return Super::GetNodeDescription();
}

EDataValidationResult UFN_ProgressTask::ValidateNode()
{
	bool FailedValidation = false;
	
	if(!Task.IsValid())
	{
		ValidationLog.Error<UFlowNode>(TEXT("Missing Task"), this);
		FailedValidation = true;
	}

	if(ProgressToAdd == 0)
	{
		ValidationLog.Error<UFlowNode>(TEXT("Missing Task"), this);
		FailedValidation = true;
	}

	if(FailedValidation)
	{
		return EDataValidationResult::Invalid;
	}
	return EDataValidationResult::Valid;
}
#endif
