// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

class FS_FactCustomization : public IPropertyTypeCustomization
{
public:
	
	/**It is just a convenient helpers which will be used
	 * to register our customization. When the propertyEditor module
	 * find our FMyStruct property, it will use this static method
	 * to instantiate our customization object.*/
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	// BEGIN IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle,
		class FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle,
		class IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// END IPropertyTypeCustomization interface
	
};
