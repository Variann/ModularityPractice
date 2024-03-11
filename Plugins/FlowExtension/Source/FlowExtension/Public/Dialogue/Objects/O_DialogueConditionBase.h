// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"
#include "UObject/Object.h"
#include "O_DialogueConditionBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, AutoExpandCategories = ("Default"), EditInlineNew,
	meta=(ShowWorldContextPin, ContextMenuCategory = "Varian's Plugins", ContextMenuEntryName = "Dialogue|Condition", ContextMenuPrefix = "DC_"))
class FLOWEXTENSION_API UO_DialogueConditionBase : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "Dialogue", BlueprintReadOnly)
	UFlowAsset* FlowGraph = nullptr;

	UFUNCTION(Category = "Dialogue", BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	bool IsConditionMet();

	virtual UWorld* GetWorld() const override;
};
