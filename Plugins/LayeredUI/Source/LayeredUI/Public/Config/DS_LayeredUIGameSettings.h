// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Core/DA_SettingsLayeredUI.h"
#include "Engine/DeveloperSettings.h"
#include "DS_LayeredUIGameSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "LayeredUI Settings"))
class LAYEREDUI_API UDS_LayeredUIGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UDS_LayeredUIGameSettings();

public:
	
	/**The map used to figure out what ZOrder to assign a widget when adding
	 * it to the screen.*/
	UPROPERTY(Category = "Settings", Config, EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "UI Data", ForceInlineRow))
	TMap<FGameplayTag, int32> LayerMap;
};