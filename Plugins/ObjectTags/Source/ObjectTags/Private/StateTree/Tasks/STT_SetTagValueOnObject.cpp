// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "StateTree/Tasks/STT_SetTagValueOnObject.h"

#include "AC_ObjectTags.h"
#include "ObjectTags_Subsystem.h"
#include "Kismet/KismetSystemLibrary.h"

EStateTreeRunStatus USTT_SetTagValueOnObject::EnterState(FStateTreeExecutionContext& Context,
                                                         const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	
	if(!Object)
	{
		RunStatus = EStateTreeRunStatus::Failed;
		UKismetSystemLibrary::PrintString(this, "SetTagValueOnObject task had no object or could not find subsystem");
		return RunStatus;
	}

	for(auto& CurrentTagValue : TagValues)
	{
		UObjectTags_Subsystem::AddTagToObject(CurrentTagValue.Tag, Object, this, CurrentTagValue.Value, CurrentTagValue.Duration);
	}

	return EStateTreeRunStatus::Succeeded;
}
