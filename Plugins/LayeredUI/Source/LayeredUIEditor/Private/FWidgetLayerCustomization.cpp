#include "FWidgetLayerCustomization.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "EditorUtilityWidget.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyHandle.h"
#include "Data/CoreLayeredUIData.h"

#define LOCTEXT_NAMESPACE "WidgetLayer customization"

TSharedRef<IPropertyTypeCustomization> FWidgetLayerCustomization::MakeInstance()
{
	return MakeShareable(new FWidgetLayerCustomization());
}

void FWidgetLayerCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle,
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

void FWidgetLayerCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle,
	IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> TagHandle = InStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FWidgetLayer, Layer));
	TSharedPtr<IPropertyHandle> ValueHandle = InStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FWidgetLayer, ZOrder));

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
		FExecuteAction InsertAction = FExecuteAction::CreateLambda([InStructPropertyHandle]
		{
			const TSharedPtr<IPropertyHandleArray> ParentPropertyHandleArray = InStructPropertyHandle->GetParentHandle()->AsArray();
			const int32 ArrayIndex = InStructPropertyHandle->IsValidHandle() ? InStructPropertyHandle->GetIndexInArray() : INDEX_NONE;
			if (ParentPropertyHandleArray.IsValid() && ArrayIndex >= 0)
			{
				ParentPropertyHandleArray->Insert(ArrayIndex);
			}
		});

		const FExecuteAction DeleteAction = FExecuteAction::CreateLambda([InStructPropertyHandle]
		{
			const TSharedPtr<IPropertyHandleArray> ParentPropertyHandleArray = InStructPropertyHandle->GetParentHandle()->AsArray();
			const TSharedPtr<IPropertyHandleSet> ParentPropertyHandleSet = InStructPropertyHandle->GetParentHandle()->AsSet();
			const int32 ArrayIndex = InStructPropertyHandle->IsValidHandle() ? InStructPropertyHandle->GetIndexInArray() : INDEX_NONE;
			if (ParentPropertyHandleArray.IsValid() && ArrayIndex >= 0)
			{
				ParentPropertyHandleArray->DeleteItem(ArrayIndex);
			}
			else if(ParentPropertyHandleSet.IsValid() && ArrayIndex >= 0)
			{
				ParentPropertyHandleSet->DeleteItem(ArrayIndex);
			}
		});

		FExecuteAction DuplicateAction = FExecuteAction::CreateLambda([InStructPropertyHandle]
		{
			const TSharedPtr<IPropertyHandleArray> ParentPropertyHandleArray = InStructPropertyHandle->GetParentHandle()->AsArray();
			const int32 ArrayIndex = InStructPropertyHandle->IsValidHandle() ? InStructPropertyHandle->GetIndexInArray() : INDEX_NONE;
			if (ParentPropertyHandleArray.IsValid() && ArrayIndex >= 0)
			{
				ParentPropertyHandleArray->DuplicateItem(ArrayIndex);
			}
		});
		
		StructBuilder.AddCustomRow(LOCTEXT("Widget Layer", "FWidgetLayer"))
		.NameContent()
		[
			StructBuilder.GenerateStructValueWidget(TagHandle.ToSharedRef())
		]	
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				ValueHandle->CreatePropertyValueWidget()
			]
			+ SHorizontalBox::Slot()
			[
				//The Insert/Delete/Duplicate button lives in the header. Since we are removing the header, we have to add it again.
				PropertyCustomizationHelpers::MakeInsertDeleteDuplicateButton(InsertAction, DeleteAction, DuplicateAction)
			]
		];
	}
}

#undef LOCTEXT_NAMESPACEF
