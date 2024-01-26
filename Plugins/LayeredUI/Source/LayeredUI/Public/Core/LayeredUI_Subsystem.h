// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/CoreLayeredUIData.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "LayeredUI_Subsystem.generated.h"

class UWidget;
class UPanelWidget;
class UW_UI_Manager;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWidgetAdded, FLayeredWidget, WidgetLayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWidgetRemoved, FLayeredWidget, WidgetLayer);

UCLASS()
class LAYEREDUI_API ULayeredUI_Subsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

private:

	/**References to current widgets that have been added to the screen
	 * using this subsystem.*/
	UPROPERTY()
	TArray<FLayeredWidget> LayeredWidgets;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UWidget>> ActiveSlots;

	UPROPERTY()
	UW_UI_Manager* UI_Manager = nullptr;

public:

	UPROPERTY(Category = "LayeredUI", BlueprintReadWrite)
	UW_UI_Manager* UIManager;

	UPROPERTY(Category = "LayeredUI", BlueprintAssignable)
	FWidgetAdded WidgetAdded;

	UPROPERTY(Category = "LayeredUI", BlueprintAssignable)
	FWidgetRemoved WidgetRemoved;

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

	UFUNCTION(Category="LayeredUI", BlueprintCallable, meta = (CallableWithoutWorldContext, WorldContext = "WorldContextObject", DisplayName = "Get UI Manager"))
	static UW_UI_Manager* GetUIManager(const UObject* WorldContextObject, bool CreateIfMissing = true);
	
	UFUNCTION(Category="LayeredUI", BlueprintCallable, meta = (CallableWithoutWorldContext, WorldContext = "WorldContextObject", DisplayName = "Set UI Manager"))
	static void SetUIManager(const UObject* WorldContextObject, UW_UI_Manager* New_UI_Manager);

	UFUNCTION(Category="LayeredUI", BlueprintCallable, BlueprintPure, meta = (CallableWithoutWorldContext, WorldContext = "WorldContextObject"))
	static bool IsWidgetValid(const UObject* WorldContextObject, FLayeredWidget Widget);

	UFUNCTION(Category="LayeredUI", BlueprintCallable, meta = (CallableWithoutWorldContext, WorldContext = "WorldContextObject"))
	static void AddWidgetToLayer(const UObject* WorldContextObject, UUserWidget* Widget, UPARAM(meta=(Categories="LayeredUI"))FGameplayTag Layer, FLayeredWidget& LayeredWidget);
	void AddWidgetToLayer_Internal(UUserWidget* Widget, UPARAM(meta=(Categories="LayeredUI"))FGameplayTag Layer, FLayeredWidget& LayeredWidget);

	UFUNCTION(Category="LayeredUI", BlueprintCallable, meta = (CallableWithoutWorldContext, WorldContext = "WorldContextObject"))
	static void RemoveWidgetFromLayer(const UObject* WorldContextObject, UUserWidget* Widget);
	void RemoveWidgetFromLayer_Internal(UUserWidget* Widget);
	
	UFUNCTION(Category="LayeredUI", BlueprintCallable, meta = (CallableWithoutWorldContext, WorldContext = "WorldContextObject"))
	static void FindFirstWidgetOnLayer(const UObject* WorldContextObject, FGameplayTag Layer, UPARAM(meta=(Categories="LayeredUI"))FLayeredWidget& Widget);
	void FindFirstWidgetOnLayer_Internal(FGameplayTag Layer, UPARAM(meta=(Categories="LayeredUI"))FLayeredWidget& Widget);

	/**Register a panel widget as a slot, this will cause all future widgets we attempt
	 * to add to the screen to rather be added as a child to the slot, instead
	 * of being added to the viewport.
	 * Note: The widget should only be a child of UPanelSlot or UCommonActivatableWidgetContainerBase.
	 * Since CommonUI doesn't inherit from UPanelSlot and the only parent the two share is UWidget,
	 * the parameter must be UWidget.*/
	UFUNCTION(Category="LayeredUI", BlueprintCallable, meta = (CallableWithoutWorldContext, WorldContext = "WorldContextObject"))
	static void RegisterSlot(const UObject* WorldContextObject, UWidget* Slot, UPARAM(meta=(Categories="LayeredUI"))FGameplayTag Layer);

	UWidget* GetSlotForLayer(FGameplayTag Layer);
};
