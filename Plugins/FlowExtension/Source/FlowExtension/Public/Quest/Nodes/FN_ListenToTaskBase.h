// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "FN_ListenToTaskBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, AutoExpandCategories = "Task")
class FLOWEXTENSION_API UFN_ListenToTaskBase : public UFlowNode
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, meta=(Categories="Flow.Quests"))
	FGameplayTag TaskToListenTo;

#if WITH_EDITOR
public:
	virtual FString GetNodeDescription() const override;
	virtual EDataValidationResult ValidateNode() override;
#endif
};
