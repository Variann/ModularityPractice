#include "LayeredUIEditor.h"

#include "FWidgetLayerCustomization.h"
#include "PropertyEditorModule.h"
#include "Data/CoreLayeredUIData.h"

IMPLEMENT_GAME_MODULE(FLayeredUIEditorModule, TagFactsEditor)

void FLayeredUIEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//Register the FS_Fact struct customization
	PropertyModule.RegisterCustomPropertyTypeLayout(
		FWidgetLayer::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FWidgetLayerCustomization::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FLayeredUIEditorModule::ShutdownModule()
{
    
}