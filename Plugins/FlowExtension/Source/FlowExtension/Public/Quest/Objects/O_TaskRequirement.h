// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "O_TaskRequirement.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, meta=(ShowWorldContextPin), AutoExpandCategories = ("Default"), EditInlineNew)
class FLOWEXTENSION_API UO_TaskRequirement : public UObject
{
	GENERATED_BODY()
};
