// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Quest/Nodes/FN_ListenToTaskBase.h"

UFN_ListenToTaskBase::UFN_ListenToTaskBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Quest");
#endif
	
	OutputPins.Add(FFlowPin(TEXT("Completed")));
	OutputPins.Add(FFlowPin(TEXT("Incremented")));
	OutputPins.Add(FFlowPin(TEXT("Decremented")));
	OutputPins.Add(FFlowPin(TEXT("Failed")));
}

FString UFN_ListenToTaskBase::GetNodeDescription() const
{
	if(TaskToListenTo.IsValid())
	{
		return TaskToListenTo.ToString();
	}
	
	return Super::GetNodeDescription();
}

EDataValidationResult UFN_ListenToTaskBase::ValidateNode()
{
	if(!TaskToListenTo.IsValid())
	{
		ValidationLog.Error<UFlowNode>(TEXT("Missing TagToListenTo"), this);
		return EDataValidationResult::Invalid;
	}

	return EDataValidationResult::Valid;
}
