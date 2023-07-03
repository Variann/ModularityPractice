// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Quest/Nodes/FN_ProgressTask.h"

UFN_ProgressTask::UFN_ProgressTask(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Quest");
#endif
}

FString UFN_ProgressTask::GetNodeDescription() const
{
	if(Task.IsValid())
	{
		return Task.ToString();
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

	if(Progress == 0)
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
