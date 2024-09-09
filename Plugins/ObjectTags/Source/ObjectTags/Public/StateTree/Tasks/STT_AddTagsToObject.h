// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STT_AddTagsToObject.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Add tags to object", Category = "ObjectTags")
class OBJECTTAGS_API USTT_AddTagsToObject : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	UObject* Object = nullptr;
	
	UPROPERTY(EditInstanceOnly)
	FGameplayTagContainer TagsToAdd;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
};
