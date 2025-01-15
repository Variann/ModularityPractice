// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Engine/Blueprint.h"
#include "FlowNodeBlueprint.generated.h"

/**
 * Flow Node Blueprint class
 */
UCLASS(BlueprintType)
class FLOW_API UFlowNodeBlueprint : public UBlueprint
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITOR
	// UBlueprint
	virtual bool SupportedByDefaultBlueprintFactory() const override { return false; }
	virtual bool SupportsDelegates() const override { return false; }
	// --
#endif
};
