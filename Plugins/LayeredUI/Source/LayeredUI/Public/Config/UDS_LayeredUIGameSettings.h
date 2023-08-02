// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/DA_SettingsLayeredUI.h"
#include "Engine/DeveloperSettings.h"
#include "UDS_LayeredUIGameSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "LayeredUI Settings"))
class LAYEREDUI_API UUDS_LayeredUIGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "UI Data"))
	TSoftObjectPtr<UDA_SettingsLayeredUI> UIData;
};