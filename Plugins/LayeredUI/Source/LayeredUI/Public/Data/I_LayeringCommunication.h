// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CoreLayeredUIData.h"
#include "UObject/Interface.h"
#include "I_LayeringCommunication.generated.h"

class UWidget;
// This class does not need to be modified.
UINTERFACE()
class UI_LayeringCommunication : public UInterface
{
	GENERATED_BODY()
};

class LAYEREDUI_API II_LayeringCommunication
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LayeredUI||Widgets||Layering")
	void SetWidgetLayerData(FLayeredWidget WidgetLayer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LayeredUI||Widgets||Layering")
	FLayeredWidget GetWidgetLayerData();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LayeredUI||Widgets||Settings")
	bool AllowMultipleInstances();

	/**Whenever AddWidgetToLayer is called, it will first ask this function
	 * to all widgets that are registered as slots. This can be vertical boxes,
	 * canvas panels, anything that will accept a child and then attempt
	 * to add the widget as a child for that widget.*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LayeredUI||Widgets||Settings")
	UWidget* GetSlotForLayer(UPARAM(meta=(Categories="UI.Layer"))FGameplayTag Layer);

	//-------------------------
	/*Some widgets are not part of the CommonUI framework for various reasons, these two interface
	 events are meant to communicate what state a widget is in or set its state.*/

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LayeredUI||Widgets||State")
	bool DeactivateWidgetsState();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LayeredUI||Widgets||State")
	bool ActivateWidgetsState();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LayeredUI||Widgets||State")
	bool IsWidgetStateActive();
	
	//-------------------------
};
