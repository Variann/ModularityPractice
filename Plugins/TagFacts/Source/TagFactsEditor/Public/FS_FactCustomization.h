// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "DataRegistryEditorModule.h"
#include "DataRegistryId.h"
#include "GameplayTagsManager.h"
#include "GameplayTagCustomizationOptions.h"
#include "SGameplayTagWidget.h"

struct FGameplayTagContainer;

class FS_FactCustomization : public IPropertyTypeCustomization, public FEditorUndoClient
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

	/** Change value and forward to delegates */
	void OnNameSelected(const FString& NameString);

	void OnTagChanged(const FGameplayTag& NewTag);

	/** Root gameplay tag to show */
	FString CachedBaseGameplayTag;
	
	/** Tag representation of name */
	TSharedPtr<FGameplayTag> CachedTag;
	
	/** Cached Id, if this changes then refresh items */
	FDataRegistryId CachedIdValue;
	FOnSetDataRegistryId OnSetId;

};
