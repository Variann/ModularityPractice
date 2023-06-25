// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "O_TaskFailCondition.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, meta=(ShowWorldContextPin), AutoExpandCategories = ("Default"), EditInlineNew)
class FLOWEXTENSION_API UO_TaskFailCondition : public UObject
{
	GENERATED_BODY()
};
