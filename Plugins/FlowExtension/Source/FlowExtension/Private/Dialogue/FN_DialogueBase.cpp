// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/FN_DialogueBase.h"

#include "Objects/O_DialogueOverrideBase.h"

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

	if(DialogueOptions.IsEmpty())
	{
		ValidationLog.Error<UFlowNode>(TEXT("No dialogue options"), this);
		FailedValidation = true;
	}
	else
	{
		for(auto& CurrentOption : DialogueOptions)
		{
			if(CurrentOption.DialogueSettings.ButtonText.IsEmpty())
			{
				ValidationLog.Error<UFlowNode>(TEXT("Dialogue option has no button text"), this);
				FailedValidation = true;
			}

			if(CurrentOption.DialogueSettings.DialogueText.IsEmpty())
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
		FS_DialogueSettings DialogueSettings = GetSettingsForDialogue(Dialogue);

		FText PinText = DialogueSettings.ButtonText;
		// Dialogue.DialogueSettings.
		OutputPins.Add(FFlowPin(PinText.ToString(), FText(), DialogueSettings.DialogueText.ToString()));
	}
}
#endif

void UFN_DialogueBase::Stop() const
{
}

FS_Script UFN_DialogueBase::GetScript()
{
	return Script;
}

TArray<FS_DialogueOption> UFN_DialogueBase::GetDialogueOptions()
{
	return DialogueOptions;
}


FS_DialogueSettings UFN_DialogueBase::GetSettingsForDialogue(FS_DialogueOption DialogueOption)
{
	bool bInEditor = false;
	
#if WITH_EDITOR
	
	bInEditor = true;

#endif
	
	if(bInEditor)
	{
		return DialogueOption.DialogueSettings;
	}
	
	for(auto& CurrentOverride : DialogueOption.OptionOverrides)
	{
		if(IsValid(CurrentOverride))
		{
			if(CurrentOverride->IsOverrideConditionMet())
			{
				return CurrentOverride->NewDialogueOption;
			}
		}
	}

	return DialogueOption.DialogueSettings;
}

UTexture2D* UFN_DialogueBase::GetSpeakerPortrait_Implementation()
{
	if(IsValid(Character))
	{
		return Character->FlowNodePortrait;
	}
	return nullptr;
}

FText UFN_DialogueBase::GetReadableDialogueString_Implementation()
{

	bool bInEditor = false;
	
#if WITH_EDITOR
	
	bInEditor = true;

#endif

	if(bInEditor)
	{
		if(!Script.DialogueText.IsEmpty())
		{
			return Script.DialogueText;
		}
		else
		{
			return FText(FText::FromString("No text set"));
		}
	}
	
	for(auto& CurrentOverride : Script.OptionOverrides)
	{
		if(IsValid(CurrentOverride))
		{
			if(CurrentOverride->IsOverrideConditionMet())
			{
				return CurrentOverride->NewDialogueOption.DialogueText;
			}
		}
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
	if(IsValid(Character))
	{
		return Character->NodeColor;
	}
	
	return FLinearColor();
}
