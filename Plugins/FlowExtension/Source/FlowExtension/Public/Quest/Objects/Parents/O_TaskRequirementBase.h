// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "O_TaskRequirementBase.generated.h"

class UQuestSubSystem;
/**
 * Base class to create requirements for a task.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, AutoExpandCategories = ("Default"), EditInlineNew,
	meta=(ContextMenuCategory = "Varian's Plugins", ContextMenuEntryName = "Quest|Task Requirement", ContextMenuPrefix = "QR_"))
class FLOWEXTENSION_API UO_TaskRequirementBase : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(Category = "Task Requirement", BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	bool IsConditionMet();

	virtual class UWorld* GetWorld() const override;
};
