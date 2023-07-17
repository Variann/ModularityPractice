// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "FS_FactCustomization.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Data/CoreTagFactData.h"


#define LOCTEXT_NAMESPACE "MyGameEditor"

TSharedRef<IPropertyTypeCustomization> FS_FactCustomization::MakeInstance()
{
	return MakeShareable(new FS_FactCustomization());
}

void FS_FactCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle,
                                           FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	
}

void FS_FactCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle,
	IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> TagHandle = InStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FS_Fact, Tag));
	TSharedPtr<IPropertyHandle> ValueHandle = InStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FS_Fact, Value));

	StructBuilder.AddCustomRow(LOCTEXT("Fact", "FS_Fact")).NameContent()[
		StructBuilder.GenerateStructValueWidget(TagHandle.ToSharedRef())].ValueContent()[
			ValueHandle->CreatePropertyValueWidget()];
}


#undef LOCTEXT_NAMESPACE
