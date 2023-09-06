// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FL_ColorTags.generated.h"

/**
 * 
 */
UCLASS()
class COLORTAGS_API UFL_ColorTags : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(Category = "Color Attributes", BlueprintPure, BlueprintCallable)
	static FLinearColor GetColorAttribute(FGameplayTag Tag, bool& ColorFound);
};
