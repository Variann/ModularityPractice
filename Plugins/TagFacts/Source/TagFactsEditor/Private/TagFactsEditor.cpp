#include "TagFactsEditor.h"

#include "FS_FactCustomization.h"
#include "GameplayTagsManager.h"
#include "Data/CoreTagFactData.h"

IMPLEMENT_GAME_MODULE(FTagFactsEditorModule, TagFactsEditor)

#define LOCTEXT_NAMESPACE "FTagFactsEditorModule"

void FTagFactsEditorModule::StartupModule()
{
	// import the PropertyEditor module...
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	// to register our custom property
	PropertyModule.RegisterCustomPropertyTypeLayout(
		// This is the name of the Struct (we can also use "MyStruct" instead)
		// this tells the property editor which is the struct property our customization will applied on.
		FS_Fact::StaticStruct()->GetFName(),
		// this is where our MakeInstance() method is usefull
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FS_FactCustomization::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FTagFactsEditorModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE