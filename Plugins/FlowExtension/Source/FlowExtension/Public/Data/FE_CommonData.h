// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FE_CommonData.generated.h"

class UO_DialogueOverrideBase;

USTRUCT(BlueprintType)
struct FS_FlowPin
{
	GENERATED_BODY()

	// A logical name, used during execution of pin
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowPin")
	FName PinName;

	// An optional Display Name, you can use it to override PinName without the need to update graph connections
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowPin")
	FText PinFriendlyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowPin")
	FString PinToolTip;
};

USTRUCT(BlueprintType)
struct FS_DialogueSettings
{
	GENERATED_BODY()

	/**The text displayed to the player to choose this option.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FText ButtonText;

	/**The actual dialogue that occurs when the player presses
	 * the dialogue button.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FText DialogueText;
};

USTRUCT(BlueprintType)
struct FS_DialogueOption
{
	GENERATED_BODY()
	
	/**The actual dialogue that occurs when the player presses
	 * the dialogue button.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FS_DialogueSettings DialogueSettings;

	/**If any of these objects override conditions return true, their dialogue
	 * settings will be used.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Dialogue Option")
	TArray<UO_DialogueOverrideBase*> OptionOverrides;
};

USTRUCT(BlueprintType)
struct FS_Script
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FText DialogueText;

	/**If any of these objects override conditions return true, their dialogue
	 * settings will be used.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Dialogue Option")
	TArray<UO_DialogueOverrideBase*> OptionOverrides;
};