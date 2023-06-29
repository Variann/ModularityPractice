// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Quest/FL_QuestHelpers.h"

#include "FlowAsset.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Quest/Nodes/FN_QuestBase.h"

UFlowNode* UFL_QuestHelpers::GetQuestNode(UFlowAsset* Graph, FGameplayTag QuestID)
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
			if(QuestNode->QuestInformation.QuestID == QuestID)
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

FS_QuestWrapper UFL_QuestHelpers::WrapQuest(FS_Quest QuestInformation, UFN_QuestBase* QuestNode)
{
	FS_QuestWrapper QuestWrapper;

	if(IsValid(QuestNode))
	{
		QuestWrapper.Graph = QuestNode->GetFlowAsset();
		QuestWrapper.ParentNode = QuestNode;
	}
	
	QuestWrapper.QuestID = QuestInformation.QuestID;
	QuestWrapper.QuestName = QuestInformation.QuestName;
	QuestWrapper.Requirements = QuestInformation.Requirements;
	QuestWrapper.FailConditions = QuestInformation.FailConditions;
	QuestWrapper.Rewards = QuestInformation.Rewards;
	QuestWrapper.State = InProgress;

	return QuestWrapper;
}

FS_TaskWrapper UFL_QuestHelpers::WrapTask(FS_QuestTask TaskInformation)
{
	FS_TaskWrapper TaskWrapper;
	
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
