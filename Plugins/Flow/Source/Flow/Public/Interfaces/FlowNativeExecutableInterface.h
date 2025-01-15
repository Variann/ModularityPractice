// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UObject/Interface.h"

#include "FlowNativeExecutableInterface.generated.h"

UINTERFACE(DisplayName = "[DEPRECATED] Flow Native Executable Interface", meta = (CannotImplementInterfaceInBlueprint, Deprecated))
class UFlowNativeExecutableInterface : public UInterface
{
	GENERATED_BODY()
};

class IFlowNativeExecutableInterface
{
	GENERATED_BODY()

public:

	// NOTE (gtaylor) All of these functions have been moved into UFlowNodeBase.
	// Keeping the empty interface existing for a time until all of the assets are updated,
	// to prevent an assert.
};
