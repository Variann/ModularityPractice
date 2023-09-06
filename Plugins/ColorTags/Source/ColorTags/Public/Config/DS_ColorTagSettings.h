// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "DS_ColorTagSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "Color Tag Settings"))
class COLORTAGS_API UDS_ColorTagSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
	UDS_ColorTagSettings();

public:

	UPROPERTY(Category = "Settings", Config, EditAnywhere, BlueprintReadOnly, meta = (ForceInlineRow))
	TMap<FGameplayTag, FLinearColor> ColorTags;

	UFUNCTION(Category = "Color Attributes", BlueprintPure, BlueprintCallable)
	static FLinearColor GetColorByTag(FGameplayTag Tag, bool& ColorFound);
};
