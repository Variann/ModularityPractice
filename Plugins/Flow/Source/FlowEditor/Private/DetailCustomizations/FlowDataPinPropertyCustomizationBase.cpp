// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "DetailCustomizations/FlowDataPinPropertyCustomizationBase.h"
#include "DetailWidgetRow.h"

void FFlowDataPinPropertyCustomizationBase::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	uint32 NumChildren = 0;
	InStructPropertyHandle->GetNumChildren(NumChildren);

	for (uint32 ChildNum = 0; ChildNum < NumChildren; ++ChildNum)
	{
		TSharedPtr<IPropertyHandle> ChildPtr = InStructPropertyHandle->GetChildHandle(ChildNum);

		HeaderRow.NameContent()
			[
				InStructPropertyHandle->CreatePropertyNameWidget()
			];
		HeaderRow.ValueContent()
			[
				ChildPtr->CreatePropertyValueWidget()
			];

		// Use the 0th child's Value Widget to replace the Header row's Value Widget
		break;
	}
}

void FFlowDataPinPropertyCustomizationBase::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// Do not display any children
}
