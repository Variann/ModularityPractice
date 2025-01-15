// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UnrealExtensions/IFlowCuratedNamePropertyCustomization.h"

#include "Types/FlowDataPinProperties.h"

// NOTE (gtaylor) this is nearly identical to AI Flow - FConfigurableEnumPropertyCustomization, can we combine them?

class FFlowDataPinProperty_EnumCustomizationBase : public IFlowCuratedNamePropertyCustomization
{
private:
	typedef IFlowCuratedNamePropertyCustomization Super;

protected:

	//~Begin IPropertyTypeCustomization
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	//~End IPropertyTypeCustomization

	//~Begin ICuratedNamePropertyCustomization
	virtual TSharedPtr<IPropertyHandle> GetCuratedNamePropertyHandle() const override;
	virtual void SetCuratedName(const FName& NewName) override;
	virtual bool TryGetCuratedName(FName& OutName) const override;
	virtual TArray<FName> GetCuratedNameOptions() const override;
	virtual bool AllowNameNoneIfOtherOptionsExist() const override { return false; }
	//~End ICuratedNamePropertyCustomization

	// Accessor to return the actual struct being edited
	FORCEINLINE FFlowDataPinOutputProperty_Enum* GetFlowDataPinEnumProperty() const
	{
		return IFlowExtendedPropertyTypeCustomization::TryGetTypedStructValue<FFlowDataPinOutputProperty_Enum>(StructPropertyHandle);
	}

	void OnEnumNameChanged();

	const UEnum* GetEnumClass() const;

	static TArray<FName> GetEnumValues(const UEnum& Enum);
};

// Details customization for FFlowDataPinOutputProperty_Enum
template <typename TEnumProperty>
class TFlowDataPinProperty_EnumCustomization : public FFlowDataPinProperty_EnumCustomizationBase
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance() { return MakeShareable(new TFlowDataPinProperty_EnumCustomization<TEnumProperty>()); }
};

// Details customization for enum FFlowDataPinProperties
typedef TFlowDataPinProperty_EnumCustomization<FFlowDataPinOutputProperty_Enum> FFlowDataPinOutputProperty_EnumCustomization;
typedef TFlowDataPinProperty_EnumCustomization<FFlowDataPinInputProperty_Enum> FFlowDataPinInputProperty_EnumCustomization;
