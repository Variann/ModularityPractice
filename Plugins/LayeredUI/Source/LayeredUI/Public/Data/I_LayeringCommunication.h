// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CoreLayeredUIData.h"
#include "UObject/Interface.h"
#include "I_LayeringCommunication.generated.h"

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LayeredUI||Widgets||Cursor")
	bool HideCursor();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LayeredUI||Widgets||Layering")
	void SetWidgetLayerData(FLayeredWidget WidgetLayer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LayeredUI||Widgets||Layering")
	FLayeredWidget GetWidgetLayerData();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LayeredUI||Widgets||Settings")
	bool AllowMultipleInstances();

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
