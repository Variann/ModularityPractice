// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/FE_CommonData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Nodes/FlowNode.h"
#include "FL_QuestHelpers.generated.h"

class UFN_QuestBase;
/**
 * 
 */
UCLASS()
class FLOWEXTENSION_API UFL_QuestHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static UFlowNode* GetQuestNode(UFlowAsset* Graph, FGameplayTag QuestID);

	UFUNCTION(BlueprintCallable)
	static TArray<UFlowNode*> GetFlowNodes(UFlowAsset* FlowAsset);

	/**Wrap a quest into the struct the quest component will use.
	 *
	 * @QuestNode This is optional.*/
	UFUNCTION(BlueprintCallable)
	static FS_QuestWrapper WrapQuest(FS_Quest QuestInformation, UFN_QuestBase* QuestNode);

	UFUNCTION(BlueprintCallable)
	static FS_TaskWrapper WrapTask(FS_QuestTask TaskInformation);
};
