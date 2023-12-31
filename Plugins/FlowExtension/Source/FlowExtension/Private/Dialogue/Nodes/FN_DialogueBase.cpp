﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/Nodes/FN_DialogueBase.h"

const FString UFN_DialogueBase::Continue = TEXT("Continue");

UFN_DialogueBase::UFN_DialogueBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Dialogue");
	RefreshOutputs();
#endif
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

EDataValidationResult UFN_DialogueBase::ValidateNode()
{
	bool FailedValidation = false;
	
	if(!IsValid(Character))
	{
		ValidationLog.Error<UFlowNode>(TEXT("Missing Character data"), this);
		FailedValidation = true;
	}

	if(Script.DialogueText.IsEmpty())
	{
		ValidationLog.Error<UFlowNode>(TEXT("No dialogue text"), this);
		FailedValidation = true;
	}

	if(!DialogueOptions.IsEmpty())
	{
		for(auto& CurrentOption : DialogueOptions)
		{
			if(CurrentOption.ButtonText.IsEmpty())
			{
				ValidationLog.Error<UFlowNode>(TEXT("Dialogue option has no button text"), this);
				FailedValidation = true;
			}

			if(CurrentOption.DialogueText.IsEmpty())
			{
				ValidationLog.Error<UFlowNode>(TEXT("Dialogue has no dialogue text"), this);
				FailedValidation = true;
			}
		}
	}

	if(FailedValidation)
	{
		return EDataValidationResult::Invalid;
	}
	
	return EDataValidationResult::Valid;
}

void UFN_DialogueBase::RefreshOutputs()
{
	OutputPins.Empty();

	// UpdateCustomOutputPins();

	if (!DialogueOptions.Num())
		OutputPins.Add({Continue});

	for (auto& Dialogue : DialogueOptions)
	{
		FText PinText = Dialogue.ButtonText;
		// Dialogue.DialogueSettings.
		OutputPins.Add(FFlowPin(PinText.ToString(), FText(), Dialogue.DialogueText.ToString()));
	}
}
#endif

void UFN_DialogueBase::Stop() const
{
}

FScript UFN_DialogueBase::GetScript()
{
	return Script;
}

TArray<FDialogueOption> UFN_DialogueBase::GetDialogueOptions()
{
	return DialogueOptions;
}

UTexture2D* UFN_DialogueBase::GetSpeakerPortrait_Implementation()
{
#if WITH_EDITOR
	if(IsValid(Character))
	{
		return Character->FlowNodePortrait;
	}
#endif
	return nullptr;
}

FText UFN_DialogueBase::GetReadableDialogueString_Implementation()
{
	if(!Script.DialogueText.IsEmpty())
	{
		return Script.DialogueText;
	}
	
	return FText(FText::FromString("No text set"));
}

FText UFN_DialogueBase::GetSpeakerName_Implementation()
{
	if(IsValid(Character))
	{
		return Character->Name;
	}
	
	return FText(FText::FromString("No Name"));
}

FLinearColor UFN_DialogueBase::GetSpeakerColor_Implementation()
{
#if WITH_EDITOR
	if(IsValid(Character))
	{
		return Character->NodeColor;
	}
#endif
	
	return FLinearColor();
}
