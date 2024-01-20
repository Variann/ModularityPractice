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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LayeredUI||Widgets||Settings")
	bool AllowMultipleInstances();
};
