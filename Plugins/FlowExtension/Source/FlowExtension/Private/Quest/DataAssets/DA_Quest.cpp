// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#include "Quest/DataAssets/DA_Quest.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include "Quest/FE_QuestData.h"
#include "Quest/DataAssets/DA_QuestChain.h"

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

#if WITH_EDITOR

EDataValidationResult UDA_Quest::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
	
	for(auto& CurrentChain : QuestChains)
	{
		bool SelfFound = false;
		for(auto& CurrentStage : CurrentChain->Stages)
		{
			if(CurrentStage.Quests.Contains(this))
			{
				SelfFound = true;
				break;
			}
		}
		
		if(!SelfFound)
		{
			if(!SelfFound)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("%s does not reference this quest in any of its stages."), *CurrentChain.GetAssetName())));
			}
		}
	}

	if(!QuestID.IsValid())
	{
		Context.AddError(FText::FromString("QuestID is empty"));
	}

	if(QuestName.IsEmpty())
	{
		Context.AddError(FText::FromString("Quest name is empty"));
	}

	if(Tasks.IsEmpty())
	{
		Context.AddError(FText::FromString("Quest has no tasks to complete"));
	}
	else
	{
		bool NonOptionalTaskFound = false;
		for(auto& CurrentTask : Tasks)
		{
			if(!CurrentTask.IsOptional)
			{
				NonOptionalTaskFound = true;
				break;
			}
		}
		if(!NonOptionalTaskFound)
		{
			Context.AddError(FText::FromString("All tasks are assigned as `IsOptional` which is not allowed. Must contain at least one completable task."));
		}
	}

	
	
	return Result;
}

void UDA_Quest::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	OnDataAssetChanged.Broadcast();
}

#endif
