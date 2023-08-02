// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "LayeredUI_Subsystem.generated.h"

/**
 * 
 */
UCLASS()
class LAYEREDUI_API ULayeredUI_Subsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	UFUNCTION(Category="LayeredUI", BlueprintCallable)
	TMap<FGameplayTag, int32> GetLayerMap();
};
