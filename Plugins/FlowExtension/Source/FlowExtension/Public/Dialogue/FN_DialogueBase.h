﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/FE_CommonData.h"
#include "DataAssets/DA_DialogueCharacter.h"
#include "Nodes/FlowNode.h"
#include "FN_DialogueBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, DisplayName = "Base Dialogue Node")
class FLOWEXTENSION_API UFN_DialogueBase : public UFlowNode
{
	GENERATED_UCLASS_BODY()

protected:

	UPROPERTY(EditAnywhere, Category=Dialogue)
	UDA_DialogueCharacter* Character = nullptr;
	
	UPROPERTY(EditAnywhere, Category=Dialogue)
	FS_Script Script;

	UPROPERTY(EditAnywhere, Category=Dialogue, BlueprintReadOnly)
	TArray<FS_DialogueOption> DialogueOptions;

public:
	static const FString Continue;

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void FixNode(UEdGraphNode* NewGraph) override;

	virtual EDataValidationResult ValidateNode() override;

private:
	void RefreshOutputs();

#endif

protected:
	virtual void Stop() const;


public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FS_Script GetScript();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FS_DialogueOption> GetDialogueOptions();

	UFUNCTION(BlueprintNativeEvent)
	UTexture2D* GetSpeakerPortrait();

	UFUNCTION(BlueprintNativeEvent)
	FText GetReadableDialogueString();

	UFUNCTION(BlueprintNativeEvent)
	FText GetSpeakerName();

	UFUNCTION(BlueprintNativeEvent)
	FLinearColor GetSpeakerColor();
};
