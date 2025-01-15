// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "InstancedStruct.h"
#include "UObject/Interface.h"

#include "FlowDataPinPropertyProviderInterface.generated.h"

struct FFlowDataPinProperty;

// Interface to define a FFlowDataPinProperty provider.
// This is used in plumbing data in the AI Flow extension plugin into the Flow Data Pins framework.
UINTERFACE(MinimalAPI, NotBlueprintable)
class UFlowDataPinPropertyProviderInterface : public UInterface
{
	GENERATED_BODY()
};

class FLOW_API IFlowDataPinPropertyProviderInterface
{
	GENERATED_BODY()

public:

	// Provide a FFlowDataPinProperty (instancedStruct) for the creation of data pins and supplying their values.
	virtual bool TryProvideFlowDataPinProperty(const bool bIsInputPin, TInstancedStruct<FFlowDataPinProperty>& OutFlowDataPinProperty) const = 0;
};
