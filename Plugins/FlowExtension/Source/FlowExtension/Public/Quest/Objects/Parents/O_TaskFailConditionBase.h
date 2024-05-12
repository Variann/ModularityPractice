// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "O_TaskFailConditionBase.generated.h"

class UQuestSubSystem;
/**
 * Base class to create fail conditions for tasks.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, AutoExpandCategories = ("Default"), EditInlineNew,
	meta=(ShowWorldContextPin, ContextMenuCategory = "Varian's Plugins", ContextMenuEntryName = "Quest|Task Fail Condition", ContextMenuPrefix = "QFC_"))
class FLOWEXTENSION_API UO_TaskFailConditionBase : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(Category = "Task Fail Condition", BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	bool IsTaskFailed();
};
