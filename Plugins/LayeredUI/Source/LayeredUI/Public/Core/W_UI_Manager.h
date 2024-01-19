// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Data/I_LayeringCommunication.h"
#include "W_UI_Manager.generated.h"

/**
 * A singleton widget that is created whenever the LayeredUI
 * subsystem is created.
 * This is meant  to act similar to the UI manager in Lyra,
 * a one-stop shop to manage the games overall UI.
 */
UCLASS(Abstract)
class LAYEREDUI_API UW_UI_Manager : public UUserWidget, public II_LayeringCommunication
{
	GENERATED_BODY()
	
};
