// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UnrealExtensions/IFlowExtendedPropertyTypeCustomization.h"

// Base class customization for most FFlow*DataPinProperty structs
class FFlowDataPinPropertyCustomizationBase : public IFlowExtendedPropertyTypeCustomization
{
	typedef IFlowExtendedPropertyTypeCustomization Super;

protected:

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
};

// Template to create customization classes for FFlow*DataPinProperty structs
template <class T>
class TFlowDataPinPropertyCustomization : public FFlowDataPinPropertyCustomizationBase
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance() { return MakeShareable(new TFlowDataPinPropertyCustomization<T>()); }
};
