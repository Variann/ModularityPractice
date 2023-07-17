#include "TagFactsEditor.h"

#include "FS_FactCustomization.h"
#include "GameplayTagsManager.h"
#include "Data/CoreTagFactData.h"

IMPLEMENT_GAME_MODULE(FTagFactsEditorModule, TagFactsEditor)

#define LOCTEXT_NAMESPACE "FTagFactsEditorModule"

void FTagFactsEditorModule::StartupModule()
{

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//Register the FS_Fact struct customization
	PropertyModule.RegisterCustomPropertyTypeLayout(
		FS_Fact::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FS_FactCustomization::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FTagFactsEditorModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE