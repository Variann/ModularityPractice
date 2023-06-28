// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Nodes/FlowNode.h"
#include "FL_QuestHelpers.generated.h"

/**
 * 
 */
UCLASS()
class FLOWEXTENSION_API UFL_QuestHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static UFlowNode* GetQuestNode(UFlowAsset* Graph, FGameplayTag QuestID);

	UFUNCTION(BlueprintCallable)
	static TArray<UFlowNode*> GetFlowNodes(UFlowAsset* FlowAsset);
};
