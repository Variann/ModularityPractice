// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UnrealExtensions/IFlowExtendedPropertyTypeCustomization.h"

#include "Types/FlowDataPinProperties.h"

class FFlowDataPinProperty_ObjectCustomizationBase : public IFlowExtendedPropertyTypeCustomization
{
private:
	typedef IFlowExtendedPropertyTypeCustomization Super;

protected:

	//~Begin IPropertyTypeCustomization
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	//~End IPropertyTypeCustomization

	// Accessor to return the actual struct being edited
	FORCEINLINE FFlowDataPinOutputProperty_Object* GetFlowDataPinObjectProperty() const
	{
		return IFlowExtendedPropertyTypeCustomization::TryGetTypedStructValue<FFlowDataPinOutputProperty_Object>(StructPropertyHandle);
	}

	UClass* DeriveBestClassFilter() const;
	UClass* BuildMetaClass() const;

	void TrySetClassFilterFromMetaData();

	void OnClassFilterChanged();
	void OnObjectValueChanged();

	mutable TWeakObjectPtr<UClass> CachedMetaClassPtr; 
};

// Details customization for FFlowDataPinOutputProperty_Object/FFlowDataPinInputProperty_Object
template <typename TObjectProperty>
class TFlowDataPinProperty_ObjectCustomization : public FFlowDataPinProperty_ObjectCustomizationBase
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance() { return MakeShareable(new TFlowDataPinProperty_ObjectCustomization<TObjectProperty>()); }
};

// Details customization for Object FFlowDataPinProperties
typedef TFlowDataPinProperty_ObjectCustomization<FFlowDataPinOutputProperty_Object> FFlowDataPinOutputProperty_ObjectCustomization;
typedef TFlowDataPinProperty_ObjectCustomization<FFlowDataPinInputProperty_Object> FFlowDataPinInputProperty_ObjectCustomization;
