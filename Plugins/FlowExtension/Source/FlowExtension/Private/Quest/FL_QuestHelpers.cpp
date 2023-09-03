// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Quest/FL_QuestHelpers.h"

#include "FlowAsset.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Quest/Nodes/FN_QuestBase.h"

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
	QuestWrapper.Requirements = QuestAsset->Requirements;
	QuestWrapper.FailConditions = QuestAsset->FailConditions;
	QuestWrapper.Rewards = QuestAsset->Rewards;
	QuestWrapper.State = InProgress;

	return QuestWrapper;
}

FTaskWrapper UFL_QuestHelpers::WrapTask(FQuestTask TaskInformation)
{
	FTaskWrapper TaskWrapper;
	
	TaskWrapper.TaskID = TaskInformation.TaskID;
	TaskWrapper.TaskName = TaskInformation.TaskName;
	TaskWrapper.Requirements = TaskInformation.Requirements;
	TaskWrapper.FailConditions = TaskInformation.FailConditions;
	TaskWrapper.Rewards = TaskInformation.Rewards;
	TaskWrapper.State = InProgress;
	TaskWrapper.ProgressRequired = TaskInformation.ProgressRequired;
	TaskWrapper.IsOptional = TaskInformation.IsOptional;

	return TaskWrapper;
}