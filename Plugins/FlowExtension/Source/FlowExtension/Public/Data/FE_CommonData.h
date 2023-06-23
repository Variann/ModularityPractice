// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FE_CommonData.generated.h"

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
