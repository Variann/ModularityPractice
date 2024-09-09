// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeConditionBlueprintBase.h"
#include "STC_TagValueQuery.generated.h"

UENUM(BlueprintType)
enum ETagValueQueryComparitor
{
	Equals,
	LessThan,
	GreaterThan
};

USTRUCT(BlueprintType)
struct FTagValueQueryEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag Tag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Value = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<ETagValueQueryComparitor> Comparison = Equals;
};

/**
 * 
 */
UCLASS(DisplayName = "Tags value Query", Category = "ObjectTags")
class OBJECTTAGS_API USTC_TagValueQuery : public UStateTreeConditionBlueprintBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditInstanceOnly)
	UObject* Object = nullptr;

	/**If true, every single entry must return true for this
	 * condition to return true.
	 * If set to false, just one entry has to return true
	 * for this condition to return true.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool AllMustMatch = true;

	/**The tag value query the target object*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (TitleProperty = "Tag"))
	TArray<FTagValueQueryEntry> TagValueQuery;

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
