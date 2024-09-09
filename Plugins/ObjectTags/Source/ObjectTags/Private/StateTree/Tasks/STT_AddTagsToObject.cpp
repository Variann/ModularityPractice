// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "StateTree/Tasks/STT_AddTagsToObject.h"

#include "ObjectTags_Subsystem.h"
#include "Kismet/KismetSystemLibrary.h"

EStateTreeRunStatus USTT_AddTagsToObject::EnterState(FStateTreeExecutionContext& Context,
                                                     const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	
	if(!Object)
	{
		UKismetSystemLibrary::PrintString(this, "AddTagsToObject task had no valid object");
		return EStateTreeRunStatus::Failed;
	}

	for(auto& CurrentTag : TagsToAdd)
	{
		UObjectTags_Subsystem::AddTagToObject(CurrentTag, Object, this);
	}

	FinishTask();
	return EStateTreeRunStatus::Succeeded;
}
