// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Quest/FL_QuestHelpers.h"

#include "FlowAsset.h"
#include "Kismet/KismetSystemLibrary.h"
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

bool UFL_QuestHelpers::IsTaskRequirementsMet(UQuestSubSystem* QuestComponent, TArray<UO_TaskRequirementBase*> Requirements)
{
	for(auto& CurrentRequirement : Requirements)
	{
		if(IsValid(CurrentRequirement))
		{
			if(!CurrentRequirement->IsConditionMet_Implementation(QuestComponent))
			{
				return false;
			}
		}
	}

	return true;
}

bool UFL_QuestHelpers::IsTaskFailed(UQuestSubSystem* QuestComponent, TArray<UO_TaskFailConditionBase*> FailConditions)
{
	for(auto& CurrentFailContition : FailConditions)
	{
		if(IsValid(CurrentFailContition))
		{
			if(CurrentFailContition->IsTaskFailed_Implementation(QuestComponent))
			{
				return true;
			}
		}
	}

	return false;
}

