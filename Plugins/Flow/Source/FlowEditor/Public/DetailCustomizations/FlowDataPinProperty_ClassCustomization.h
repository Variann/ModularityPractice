// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UnrealExtensions/IFlowExtendedPropertyTypeCustomization.h"

#include "Types/FlowDataPinProperties.h"

class SClassPropertyEntryBox;

class FFlowDataPinProperty_ClassCustomizationBase : public IFlowExtendedPropertyTypeCustomization
{
private:
	typedef IFlowExtendedPropertyTypeCustomization Super;

protected:

	//~Begin IPropertyTypeCustomization
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	//~End IPropertyTypeCustomization

	// Accessor to return the actual struct being edited
	FORCEINLINE FFlowDataPinOutputProperty_Class* GetFlowDataPinClassProperty() const
	{
		return IFlowExtendedPropertyTypeCustomization::TryGetTypedStructValue<FFlowDataPinOutputProperty_Class>(StructPropertyHandle);
	}

	UClass* DeriveBestClassFilter() const;
	UClass* BuildMetaClass() const;

	void TrySetClassFilterFromMetaData();

	void OnClassFilterChanged();
	const UClass* OnGetClass() const;
	void OnSetClass(const UClass* NewClass);

	mutable TWeakObjectPtr<UClass> CachedClassPtr;
	mutable TWeakObjectPtr<UClass> CachedMetaClassPtr; 
};

// Details customization for FFlowDataPinOutputProperty_Class/FFlowDataPinInputProperty_Class
template <typename TClassProperty>
class TFlowDataPinProperty_ClassCustomization : public FFlowDataPinProperty_ClassCustomizationBase
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance() { return MakeShareable(new TFlowDataPinProperty_ClassCustomization<TClassProperty>()); }
};

// Details customization for Class FFlowDataPinProperties
typedef TFlowDataPinProperty_ClassCustomization<FFlowDataPinOutputProperty_Class> FFlowDataPinOutputProperty_ClassCustomization;
typedef TFlowDataPinProperty_ClassCustomization<FFlowDataPinInputProperty_Class> FFlowDataPinInputProperty_ClassCustomization;
