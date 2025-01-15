// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "DetailCustomizations/FlowNamedDataPinOutputPropertyCustomization.h"

FText FFlowNamedDataPinOutputPropertyCustomization::BuildHeaderText() const
{
	if (const FFlowNamedDataPinOutputProperty* FlowNamedDataPinOutputProperty = IFlowExtendedPropertyTypeCustomization::TryGetTypedStructValue<FFlowNamedDataPinOutputProperty>(StructPropertyHandle))
	{
		return FlowNamedDataPinOutputProperty->BuildHeaderText();
	}

	return Super::BuildHeaderText();
}
