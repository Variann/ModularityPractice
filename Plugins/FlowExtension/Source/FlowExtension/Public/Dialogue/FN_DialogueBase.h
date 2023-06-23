// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/FE_CommonData.h"
#include "Nodes/FlowNode.h"
#include "FN_DialogueBase.generated.h"

/**
 * 
 */
UCLASS()
class FLOWEXTENSION_API UFN_DialogueBase : public UFlowNode
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=Dialogue)
	FText Text;
	UPROPERTY(EditAnywhere, Category=Dialogue)
	TArray<FText> Answers;

public:
	static const FString Continue;

	explicit UFN_DialogueBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ExecuteInput(const FName& PinName) override;

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void FixNode(UEdGraphNode* NewGraph) override;

#endif

private:
	void RefreshOutputs();
protected:
	virtual void Stop() const;


public:

	UFUNCTION(BlueprintNativeEvent)
	UTexture2D* GetSpeakerPortrait();

	UFUNCTION(BlueprintNativeEvent)
	FText GetReadableDialogueString();

	UFUNCTION(BlueprintNativeEvent)
	FText GetSpeakerName();

	UFUNCTION(BlueprintNativeEvent)
	FLinearColor GetSpeakerColor();

	// UFUNCTION(BlueprintNativeEvent)
	// TArray<FS_FlowPin> GetCustomOutputPins();
};
