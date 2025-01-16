// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Quest/FL_QuestHelpers.h"

#include "FlowAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Quest/QuestSubSystem.h"
#include "Quest/Nodes/FN_QuestBase.h"
#include "Quest/Objects/Parents/O_TaskFailConditionBase.h"
#include "Quest/Objects/Parents/O_TaskRequirementBase.h"

UFlowNode* UFL_QuestHelpers::GetQuestNode(UFlowAsset* Graph, const TSoftObjectPtr<UDA_Quest> Quest)
{
	if(!Graph->IsValidLowLevel())
	{
		return nullptr;
	}

	for(auto& CurrentNode : GetFlowNodes(Graph))
	{
		UFN_QuestBase* QuestNode = Cast<UFN_QuestBase>(CurrentNode);
		if(QuestNode)
		{
			if(QuestNode->QuestAsset == Quest)
			{
				return QuestNode;
			}
		}
	}
	
	return nullptr;
}

TArray<UFlowNode*> UFL_QuestHelpers::GetFlowNodes(UFlowAsset* FlowAsset)
{
	// UFlowGraph* NewAsset = NewObject<UFlowGraph>();
	TArray<UFlowNode*> FoundNodes;

	if(FlowAsset)
	{
		FlowAsset->PreloadNodes();
		const TMap<FGuid, UFlowNode*>& AssetNodes = FlowAsset->GetNodes();
		AssetNodes.GenerateValueArray(FoundNodes);
		
		return FoundNodes;
	}

	return FoundNodes;
}

FQuestWrapper UFL_QuestHelpers::WrapQuest(UDA_Quest* QuestAsset)
{
	FQuestWrapper QuestWrapper = FQuestWrapper();

	if(!IsValid(QuestAsset))
	{
		return QuestWrapper;
	}
	
	QuestWrapper.QuestAsset = QuestAsset;
	QuestWrapper.State = InProgress;

	return QuestWrapper;
}

FTaskWrapper UFL_QuestHelpers::WrapTask(UDA_Quest* RootQuest, FQuestTask TaskInformation)
{
	FTaskWrapper TaskWrapper;

	if(!IsValid(RootQuest))
	{
		return TaskWrapper;
	}

	TaskWrapper.RootQuest = RootQuest;
	TaskWrapper.TaskID = TaskInformation.TaskID;
	TaskWrapper.TaskName = TaskInformation.TaskName;
	TaskWrapper.State = InProgress;

	return TaskWrapper;
}

bool UFL_QuestHelpers::IsTaskRequirementsMet(TArray<UO_TaskRequirementBase*> Requirements)
{
	for(auto& CurrentRequirement : Requirements)
	{
		if(IsValid(CurrentRequirement))
		{
			if(!CurrentRequirement->IsConditionMet_Implementation())
			{
				return false;
			}
		}
	}

	return true;
}

bool UFL_QuestHelpers::IsTaskFailed(FGameplayTag Task)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return false;
	}

	FQuestWrapper FoundQuest = UQuestSubSystem::GetQuestForTask(Task);
	if(!FoundQuest.QuestAsset)
	{
		return false;
	}

	for(auto& CurrentTask : FoundQuest.Tasks)
	{
		if(CurrentTask.TaskID == Task && CurrentTask.State == Failed)
		{
			return true;
		}
	}

	return false;
}

bool UFL_QuestHelpers::ShouldTaskFail(FGameplayTag Task)
{
	UQuestSubSystem* QuestSubSystem = UQuestSubSystem::Get();
	if(!QuestSubSystem)
	{
		return false;
	}

	FQuestWrapper FoundQuest = UQuestSubSystem::GetQuestForTask(Task);
	if(!FoundQuest.QuestAsset)
	{
		return false;
	}

	for(auto& CurrentFailCondition : FoundQuest.QuestAsset.LoadSynchronous()->GetTasksFailConditions(Task))
	{
		if(IsValid(CurrentFailCondition))
		{
			if(CurrentFailCondition->IsTaskFailed_Implementation())
			{
				return true;
			}
		}
	}

	return false;
}

