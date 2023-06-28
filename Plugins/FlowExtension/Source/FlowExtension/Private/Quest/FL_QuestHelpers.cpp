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
