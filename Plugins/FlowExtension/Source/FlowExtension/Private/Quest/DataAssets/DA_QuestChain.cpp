// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Quest/DataAssets/DA_QuestChain.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif


#if WITH_EDITOR

EDataValidationResult UDA_QuestChain::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if(Stages.IsEmpty())
	{
		Context.AddError(FText::FromString("Stages is empty"));
	}
	else
	{
		for(auto& CurrentStage : Stages)
		{
			if(CurrentStage.Quests.IsEmpty())
			{
				Context.AddError(FText::FromString("Stage has no quests"));
			}

			for(auto& CurrentQuest : CurrentStage.Quests)
			{
				if(CurrentQuest.IsNull())
				{
					continue;
				}
				
				if(!CurrentQuest->QuestChains.Contains(this))
				{
					Context.AddError(FText::FromString(FString::Printf(TEXT("%s does not link back to this chain via its QuestChains array"), *CurrentQuest.GetAssetName())));
				}
			}
		}
	}
	
	return Result;
}

void UDA_QuestChain::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for(auto& CurrentStage : Stages)
	{
		for(auto& CurrentQuest : CurrentStage.Quests)
		{
			if(CurrentQuest.IsNull())
			{
				continue;
			}

			CurrentQuest.LoadSynchronous()->QuestChains.AddUnique(this);
		}
	}
}

#endif
