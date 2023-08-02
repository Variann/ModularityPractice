// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/Widget.h"
#include "Data/CoreLayeredUIData.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "LayeredUI_Subsystem.generated.h"

UCLASS()
class LAYEREDUI_API ULayeredUI_Subsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

private:

	/**References to current widgets that have been added to the screen
	 * using this subsystem.*/
	UPROPERTY()
	TArray<FLayeredWidget> LayeredWidgets;

public: 

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/**Get a copy of the layer map from the project settings.*/
	UFUNCTION(Category="LayeredUI", BlueprintCallable)
	TMap<FGameplayTag, int32> GetLayerMap();

	/**Get the references to current widgets that have been added to the screen
	 * using this subsystem.*/
	UFUNCTION(Category="LayeredUI", BlueprintCallable)
	TArray<UUserWidget*> GetCurrentWidgets();

	UFUNCTION(Category="LayeredUI", BlueprintCallable)
	TArray<FLayeredWidget> GetLayeredWidgets();

	UFUNCTION(Category="LayeredUI", BlueprintCallable)
	void AddWidgetToLayer(UUserWidget* Widget, FGameplayTag Layer, FLayeredWidget& LayeredWidget, int32 OrderOverride = -1);

	UFUNCTION(Category="LayeredUI", BlueprintCallable)
	void RemoveWidgetFromLayer(FLayeredWidget Widget, FLayeredWidget& NewFocus);
	
	UFUNCTION(Category="LayeredUI", BlueprintCallable)
	void FindFirstWidgetOnLayer(FGameplayTag Layer, FLayeredWidget& Widget);
};
