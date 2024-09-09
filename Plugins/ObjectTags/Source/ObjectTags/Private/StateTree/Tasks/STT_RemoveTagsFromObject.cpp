// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "StateTree/Tasks/STT_RemoveTagsFromObject.h"

#include "ObjectTags_Subsystem.h"
#include "Kismet/KismetSystemLibrary.h"

EStateTreeRunStatus USTT_RemoveTagsFromObject::EnterState(FStateTreeExecutionContext& Context,
                                                     const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	
	if(!Object)
	{
		RunStatus = EStateTreeRunStatus::Failed;
		UKismetSystemLibrary::PrintString(this, "AddTagsToObject task had no object or could not find subsystem");
		return RunStatus;
	}

	UObjectTags_Subsystem::RemoveTagsFromObject(TagsToRemove, Object, this);
	return EStateTreeRunStatus::Succeeded;
}
