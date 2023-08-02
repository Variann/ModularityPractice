// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "FS_FactCustomization.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "EditorUtilityWidget.h"
#include "Data/CoreTagFactData.h"


#define LOCTEXT_NAMESPACE "MyGameEditor"

TSharedRef<IPropertyTypeCustomization> FS_FactCustomization::MakeInstance()
{
	return MakeShareable(new FS_FactCustomization());
}

void FS_FactCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle,
                                           FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TArray<UObject*> OuterObjects;
	InStructPropertyHandle->GetOuterObjects(OuterObjects);

	/**Bug as of 5.2:
	 * The DetailsView widget for EditorUtilityWidgets seems to be having problems rendering
	 * widgets made through AddCustomRow. This can be validated with FRuntimeFloatCurve
	 * and FInputActionKeyMapping. If a struct using AddCustomRow is used in an array,
	 * the widget does NOT display and if there's nothing else in the widget,
	 * the expand arrow will not display.
	 *
	 * If any of the outers is a editor utility widget, fall back to the default appearance.*/
	for(auto& CurrentOuter : OuterObjects)
	{
		if(CurrentOuter->GetClass()->IsChildOf(UEditorUtilityWidget::StaticClass()))
		{
			HeaderRow.NameContent()[InStructPropertyHandle->CreatePropertyNameWidget()]
			.ValueContent()[InStructPropertyHandle->CreatePropertyValueWidget()];
			return;
		}
	}
}

void FS_FactCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle,
	IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> TagHandle = InStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FS_Fact, Tag));
	TSharedPtr<IPropertyHandle> ValueHandle = InStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FS_Fact, Value));

	TArray<UObject*> OuterObjects;
	InStructPropertyHandle->GetOuterObjects(OuterObjects);

	/**Bug as of 5.2:
	 * The DetailsView widget for EditorUtilityWidgets seems to be having problems rendering
	 * widgets made through AddCustomRow. This can be validated with FRuntimeFloatCurve
	 * and FInputActionKeyMapping. If a struct using AddCustomRow is used in an array,
	 * the widget does NOT display and if there's nothing else in the widget,
	 * the expand arrow will not display.
	 *
	 * If any of the outers is a editor utility widget, fall back to the default appearance.*/
	bool UseNormalStruct = false;
	
	for(auto& CurrentOuter : OuterObjects)
	{
		if(CurrentOuter->GetClass()->IsChildOf(UEditorUtilityWidget::StaticClass()))
		{
			UseNormalStruct = true;
		}
	}

	if(UseNormalStruct)
	{
		StructBuilder.AddProperty(TagHandle.ToSharedRef());
		StructBuilder.AddProperty(ValueHandle.ToSharedRef());
	}
	else
	{
		StructBuilder.AddCustomRow(LOCTEXT("Fact", "FS_Fact")).NameContent()[
		StructBuilder.GenerateStructValueWidget(TagHandle.ToSharedRef())].ValueContent()[
			ValueHandle->CreatePropertyValueWidget()];
	}
}

#undef LOCTEXT_NAMESPACE
