// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"
#include "UObject/Object.h"
#include "O_DialogueConditionBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, meta=(ShowWorldContextPin), AutoExpandCategories = ("Default"), EditInlineNew)
class FLOWEXTENSION_API UO_DialogueConditionBase : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "Dialogue", BlueprintReadOnly)
	UFlowAsset* FlowGraph = nullptr;

	/**If the owning dialogue option does NOT want to be hidden, but the requirements
	 * are still not met, the option will be greyed out and not selectable, but still
	 * visible to the player.*/
	UPROPERTY(Category = "Dialogue", EditAnywhere, BlueprintReadOnly)
	bool HideIfConditionIsNotMet = true;

	UFUNCTION(Category = "Dialogue", BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	bool IsConditionMet();
};
