// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STT_RemoveTagsFromObject.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Remove tags from object", Category = "ObjectTags")
class OBJECTTAGS_API USTT_RemoveTagsFromObject : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	UObject* Object = nullptr;
	
	UPROPERTY(EditInstanceOnly)
	FGameplayTagContainer TagsToRemove;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
};
