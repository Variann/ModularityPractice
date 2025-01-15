// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "DetailCustomizations/FlowDataPinProperty_EnumCustomization.h"
#include "Types/FlowDataPinProperties.h"
#include "Nodes/FlowPin.h"

#include "IDetailChildrenBuilder.h"
#include "UObject/UnrealType.h"

void FFlowDataPinProperty_EnumCustomizationBase::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	if (TSharedPtr<IPropertyHandle> EnumClassHandle = InStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFlowDataPinOutputProperty_Enum, EnumClass)))
	{
		StructBuilder.AddProperty(EnumClassHandle.ToSharedRef());
	}

	if (TSharedPtr<IPropertyHandle> EnumNameHandle = InStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFlowDataPinOutputProperty_Enum, EnumName)))
	{
		StructBuilder.AddProperty(EnumNameHandle.ToSharedRef());

		EnumNameHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FFlowDataPinProperty_EnumCustomizationBase::OnEnumNameChanged));
	}
}

TSharedPtr<IPropertyHandle> FFlowDataPinProperty_EnumCustomizationBase::GetCuratedNamePropertyHandle() const
{
	check(StructPropertyHandle->IsValidHandle());

	TSharedPtr<IPropertyHandle> FoundHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFlowDataPinOutputProperty_Enum, Value));
	check(FoundHandle);

	return FoundHandle;
}

TArray<FName> FFlowDataPinProperty_EnumCustomizationBase::GetCuratedNameOptions() const
{
	TArray<FName> Results;

	const UEnum* Enum = GetEnumClass();

	if (IsValid(Enum))
	{
		Results = GetEnumValues(*Enum);
	}

	return Results;
}

TArray<FName> FFlowDataPinProperty_EnumCustomizationBase::GetEnumValues(const UEnum& Enum)
{
	TArray<FName> EnumValues;

	for (int Index = 0; Index < Enum.GetMaxEnumValue(); Index++)
	{
		if (!Enum.IsValidEnumValue(Index))
		{
			continue;
		}

		static const TCHAR* MetaDataKey_Hidden = TEXT("Hidden");
		if (!Enum.HasMetaData(MetaDataKey_Hidden, Index))
		{
			EnumValues.Add(*Enum.GetDisplayNameTextByIndex(Index).ToString());
		}
	}

	return EnumValues;
}

void FFlowDataPinProperty_EnumCustomizationBase::SetCuratedName(const FName& NewValue)
{
	TSharedPtr<IPropertyHandle> ValueHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFlowDataPinOutputProperty_Enum, Value));

	check(ValueHandle);

	ValueHandle->SetPerObjectValue(0, NewValue.ToString());
}

bool FFlowDataPinProperty_EnumCustomizationBase::TryGetCuratedName(FName& OutName) const
{
	if (const FFlowDataPinOutputProperty_Enum* ConfigurableEnumProperty = GetFlowDataPinEnumProperty())
	{
		OutName = ConfigurableEnumProperty->Value;

		return true;
	}
	else
	{
		return false;
	}
}

void FFlowDataPinProperty_EnumCustomizationBase::OnEnumNameChanged()
{
	if (FFlowDataPinOutputProperty_Enum* FlowDataPinEnumProperty = GetFlowDataPinEnumProperty())
	{
		FlowDataPinEnumProperty->OnEnumNameChanged();
	}
}

const UEnum* FFlowDataPinProperty_EnumCustomizationBase::GetEnumClass() const
{
	if (const FFlowDataPinOutputProperty_Enum* FlowDataPinEnumProperty = GetFlowDataPinEnumProperty())
	{
		return FlowDataPinEnumProperty->EnumClass;
	}

	return nullptr;
}
