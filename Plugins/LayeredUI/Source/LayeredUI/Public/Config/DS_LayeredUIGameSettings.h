// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "..\..\Private\Data\CoreLayeredUIData.cpp"
#include "Engine/DeveloperSettings.h"
#include "DS_LayeredUIGameSettings.generated.h"

class UW_UI_Manager;
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
	UPROPERTY(Category = "Settings", Config, EditAnywhere, BlueprintReadOnly, meta = (ForceInlineRow, Categories="LayeredUI"))
	TMap<FGameplayTag, int32> LayerMap;

	UPROPERTY(Category = "Settings", Config, EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UW_UI_Manager> UIManagerClass;
	
	UPROPERTY(Category = "Settings", Config, EditAnywhere, BlueprintReadOnly)
	int32 UIManagerZOrder = 0;
};