// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FObjectTagsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	//------------------
	// Editor

private:
	FTSTicker::FDelegateHandle TickDelegateHandle;
	bool Tick(float DeltaTime);
	
	//------------------
};
