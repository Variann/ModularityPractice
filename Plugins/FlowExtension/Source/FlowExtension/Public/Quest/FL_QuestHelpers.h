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

	/**Returns true if the task has been LABELLED as failed,
	 * but does not check if the task should be labelled as failed.
	 *
	 * For example, if the task fails due to a condition not being met,
	 * lets say when you open the wrong door, there's nothing in the quest
	 * subsystem that is actively checking this. Meaning you have to both
	 * check @ShouldTaskFail and if so, call @FailTask.
	 * You can use this function to check if it has already been labelled as failed.*/
	UFUNCTION(BlueprintCallable)
	static bool IsTaskFailed(FGameplayTag Task);

	/**Checks all the fail conditions to check if the task should fail.
	 * This can return false, even though the task has been labelled as failed. */
	UFUNCTION(BlueprintCallable)
	static bool ShouldTaskFail(FGameplayTag Task);
};
