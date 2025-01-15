// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UnrealExtensions/IFlowExtendedPropertyTypeCustomization.h"

#include "Types/FlowDataPinProperties.h"

// Details customization for FFlowPin
class FFlowNamedDataPinOutputPropertyCustomization : public IFlowExtendedPropertyTypeCustomization
{
	typedef IFlowExtendedPropertyTypeCustomization Super;

public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance() { return MakeShareable(new FFlowNamedDataPinOutputPropertyCustomization()); }

protected:

	virtual FText BuildHeaderText() const override;
};
