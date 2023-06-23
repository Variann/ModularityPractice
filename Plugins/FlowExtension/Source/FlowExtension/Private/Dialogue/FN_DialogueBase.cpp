// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/FN_DialogueBase.h"

const FString UFN_DialogueBase::Continue = TEXT("Continue");

UFN_DialogueBase::UFN_DialogueBase(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Dialogue");
#endif
	RefreshOutputs();
}

void UFN_DialogueBase::ExecuteInput(const FName& PinName)
{
	// Show your Dialogue UI here
}

#if WITH_EDITOR
void UFN_DialogueBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	RefreshOutputs();
	OnReconstructionRequested.ExecuteIfBound();
	

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UFN_DialogueBase::FixNode(UEdGraphNode* NewGraph)
{
	RefreshOutputs();
}
#endif

void UFN_DialogueBase::RefreshOutputs()
{
	OutputPins.Empty();

	if (!Answers.Num())
		OutputPins.Add({Continue});

	for (FFlowPin Answer : Answers)
	{
		OutputPins.Add(Answer);
	}
}

void UFN_DialogueBase::Stop() const
{
}

// TArray<FFlowPin> UFN_DialogueBase::GetCustomOutputPins_Implementation()
// {
// 	return OutputPins;
// }

UTexture2D* UFN_DialogueBase::GetSpeakerPortrait_Implementation()
{
	return nullptr;
}

FText UFN_DialogueBase::GetReadableDialogueString_Implementation()
{
	return FText(FText::FromString("No text set"));
}

FText UFN_DialogueBase::GetSpeakerName_Implementation()
{
	return FText(FText::FromString("No Name"));
}

FLinearColor UFN_DialogueBase::GetSpeakerColor_Implementation()
{
	return FLinearColor();
}
