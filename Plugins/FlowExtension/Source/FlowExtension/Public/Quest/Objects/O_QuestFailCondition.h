// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "O_QuestFailCondition.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, meta=(ShowWorldContextPin), AutoExpandCategories = ("Default"), EditInlineNew)
class FLOWEXTENSION_API UO_QuestFailCondition : public UObject
{
	GENERATED_BODY()
};
