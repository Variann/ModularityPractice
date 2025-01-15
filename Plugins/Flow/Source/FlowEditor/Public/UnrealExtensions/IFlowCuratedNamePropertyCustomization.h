// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

// NOTE (gtaylor) This class is planned for submission to Epic to include in baseline UE.
// If/when that happens, we will want to remove this version and update to the latest one in the PropertyModule

#pragma once

#include "IFlowExtendedPropertyTypeCustomization.h"
#include "Widgets/Input/SComboBox.h"

// A base-class to do property Customization for a struct that presents a curated list of FNames for selection
class FLOWEDITOR_API IFlowCuratedNamePropertyCustomization : public IFlowExtendedPropertyTypeCustomization
{
protected:
	// IExtendedPropertyTypeCustomization
	virtual void CreateHeaderRowWidget(FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	// ---

	void Initialize();

	// Helper function to set the property to a specified value
	//  (and handle all of the side-effects)
	bool TrySetCuratedNameWithSideEffects(const FName& NewName);

	// Callbacks for the TextListWidget (see CreateHeaderRowWidget)
	FText GetCachedText() const;
	static TSharedRef<SWidget> GenerateTextListWidget(const TSharedPtr<FText> InItem);
	void OnTextListComboBoxOpening();
	void OnTextSelected(const TSharedPtr<FText> NewSelection, ESelectInfo::Type SelectInfo);

	void RepaintTextListWidget() const;

	TSharedPtr<FText> FindCachedOrCreateText(const FName& NewName);
	void AddToCachedTextList(const TSharedPtr<FText> Text);
	void InsertAtHeadOfCachedTextList(const TSharedPtr<FText> Text);

	bool CustomIsResetToDefaultVisible(TSharedPtr<IPropertyHandle> Property) const;
	void CustomResetToDefault(TSharedPtr<IPropertyHandle> Property);
	bool CustomIsEnabled() const;

	// IFlowCuratedNamePropertyCustomization
	virtual TSharedPtr<IPropertyHandle> GetCuratedNamePropertyHandle() const = 0;
	virtual void SetCuratedName(const FName& NewName) = 0;
	virtual bool TryGetCuratedName(FName& OutName) const = 0;
	virtual TArray<FName> GetCuratedNameOptions() const = 0;
	virtual bool AllowNameNoneIfOtherOptionsExist() const { return true; }
	// ---

public:
	// Cached property handle for the Curated Name property that is being customized
	TSharedPtr<IPropertyHandle> CachedNameHandle;

	// Cached PropertyUtils
	TSharedPtr<IPropertyUtilities> CachedPropertyUtils;

	// Cache FTexts for the ComboBox dropdown & current selected
	TArray<TSharedPtr<FText>> CachedTextList;
	TSharedPtr<FText> CachedTextSelected;

	// Preallocated NAME_None as FText
	static TSharedPtr<FText> NoneAsText;

	// Combo Box widget for displaying the curated list of Names
	TSharedPtr<SComboBox<TSharedPtr<FText>>> TextListWidget;
};
