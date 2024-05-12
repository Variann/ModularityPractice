// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FE_QuestData.h"
#include "Quest/DataAssets/DA_Quest.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Nodes/FlowNode.h"
#include "FL_QuestHelpers.generated.h"

class UQuestSubSystem;
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
	static UFlowNode* GetQuestNode(UFlowAsset* Graph, const TSoftObjectPtr<UDA_Quest> Quest);

	UFUNCTION(BlueprintCallable)
	static TArray<UFlowNode*> GetFlowNodes(UFlowAsset* FlowAsset);

	/**Wrap a quest into the struct the quest component will use.*/
	UFUNCTION(BlueprintCallable)
	static FQuestWrapper WrapQuest(UDA_Quest* QuestAsset);

	UFUNCTION(BlueprintCallable)
	static FTaskWrapper WrapTask(UDA_Quest* RootQuest, FQuestTask TaskInformation);

	/**Check if the task requirements are met.
	 * Will return false if any fail.*/
	UFUNCTION(BlueprintCallable)
	static bool IsTaskRequirementsMet(TArray<UO_TaskRequirementBase*> Requirements);

	/**Check if the task fail conditions are met.
	 * will return true if any succeed.*/
	UFUNCTION(BlueprintCallable)
	static bool IsTaskFailed(TArray<UO_TaskFailConditionBase*> FailConditions);
};
