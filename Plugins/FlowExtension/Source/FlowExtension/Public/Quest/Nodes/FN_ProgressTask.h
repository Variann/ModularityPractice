// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "FN_ProgressTask.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FLOWEXTENSION_API UFN_ProgressTask : public UFlowNode
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, meta=(Categories="Flow.Quests"))
	FGameplayTag Task;

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	float ProgressToAdd = 0;


#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
	virtual EDataValidationResult ValidateNode() override;
#endif
};
