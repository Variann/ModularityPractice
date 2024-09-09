// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STT_AddTagsToObjectWithRelationship.generated.h"

class UO_TagRelationship;
/**
 * 
 */
UCLASS(DisplayName = "Add tags to object with relationship", Category = "ObjectTags")
class OBJECTTAGS_API USTT_AddTagsToObjectWithRelationship : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	UObject* Object = nullptr;
	
	UPROPERTY(EditInstanceOnly)
	TArray<TSubclassOf<UO_TagRelationship>> Relationships;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
};
