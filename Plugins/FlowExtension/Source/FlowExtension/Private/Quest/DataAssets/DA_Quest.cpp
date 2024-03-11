// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#include "Quest/DataAssets/DA_Quest.h"
#include "Quest/FE_QuestData.h"

bool UDA_Quest::IsTaskOptional(FGameplayTag TaskID)
{
	for(auto& CurrentTask : Tasks)
	{
		if(CurrentTask.TaskID == TaskID)
		{
			return CurrentTask.IsOptional;
		}
	}

	return false;
}

float UDA_Quest::GetRequiredTaskProgression(FGameplayTag TaskID)
{
	for(auto& CurrentTask : Tasks)
	{
		if(CurrentTask.TaskID == TaskID)
		{
			return CurrentTask.ProgressRequired;
		}
	}

	return 0;
}

TArray<UO_TaskRequirementBase*> UDA_Quest::GetTasksRequirements(FGameplayTag TaskID)
{
	for(auto& CurrentTask : Tasks)
	{
		if(CurrentTask.TaskID == TaskID)
		{
			return CurrentTask.Requirements;
		}
	}

	TArray<UO_TaskRequirementBase*> FoundRequirements;
	return FoundRequirements;
}
