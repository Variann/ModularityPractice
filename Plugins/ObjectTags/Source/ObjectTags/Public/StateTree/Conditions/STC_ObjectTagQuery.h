// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeConditionBlueprintBase.h"

#include "STC_ObjectTagQuery.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "ObjectTags Query", Category = "ObjectTags")
class OBJECTTAGS_API USTC_ObjectTagQuery : public UStateTreeConditionBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly)
	UObject* Object = nullptr;

	UPROPERTY(EditInstanceOnly)
	FGameplayTagQuery TagQuery;
	
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
