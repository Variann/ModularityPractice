// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

struct FContextMenuEntry;

class FEditorAssistantModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**Go through all classes and find ones with the appropriate metadata,
	 * then add them to the config settings.*/
	void AutoPopulateContextMenu();
};
