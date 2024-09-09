// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "StateTree/Tasks/STT_AddTagsToObjectWithRelationship.h"

#include "ObjectTags_Subsystem.h"
#include "Kismet/KismetSystemLibrary.h"

EStateTreeRunStatus USTT_AddTagsToObjectWithRelationship::EnterState(FStateTreeExecutionContext& Context,
                                                     const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	
	if(!Object)
	{
		RunStatus = EStateTreeRunStatus::Failed;
		UKismetSystemLibrary::PrintString(this, "AddTagsToObject task had no object or could not find subsystem");
		return RunStatus;
	}

	for(auto& CurrentRelationship : Relationships)
	{
		UObjectTags_Subsystem::AddTagToObjectWithRelationship(CurrentRelationship, Object, this);
	}

	return EStateTreeRunStatus::Succeeded;
}
