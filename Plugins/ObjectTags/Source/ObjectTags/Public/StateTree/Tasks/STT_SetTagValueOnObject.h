// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STT_SetTagValueOnObject.generated.h"

struct FSimpleObjectTag;
/**
 * 
 */
UCLASS(DisplayName = "Set Tag Value on Object", Category = "ObjectTags")
class OBJECTTAGS_API USTT_SetTagValueOnObject : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	UObject* Object = nullptr;

	UPROPERTY(EditInstanceOnly)
	TArray<FSimpleObjectTag> TagValues;

	UPROPERTY(EditInstanceOnly)
	bool AddIfMissing = true;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
};
