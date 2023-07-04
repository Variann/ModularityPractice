// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/FE_CommonData.h"
#include "UObject/Object.h"
#include "O_DialogueOverrideBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, meta=(ShowWorldContextPin), AutoExpandCategories = ("Default"), EditInlineNew)
class FLOWEXTENSION_API UO_DialogueOverrideBase : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FS_DialogueSettings NewDialogueOption;

	UFUNCTION(Category = "Dialogue", BlueprintCallable, BlueprintPure, BlueprintImplementableEvent)
	bool IsOverrideConditionMet();
};
