// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Other/FN_RandomOutputWithWeights.h"

#include "Kismet/KismetMathLibrary.h"

UFN_RandomOutputWithWeights::UFN_RandomOutputWithWeights(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = "Route";
	RefreshOutputs();
#endif

	InputPins.Add(FFlowPin(TEXT("Reset Weights")));
}

FRandomOutputWithWeight UFN_RandomOutputWithWeights::ExecuteRandomOutput()
{
	float OutputsTotalSum = 0;
	for(auto& CurrentOutput : Outputs)
	{
		if(Outputs.Num() >= 3 && CurrentOutput.OutputPinName == LastOutput.OutputPinName)
		{
			continue;
		}
		
		OutputsTotalSum += CurrentOutput.CurrentWeight;
	}

	FRandomOutputWithWeight FinalOutput;
	float RandomValue = UKismetMathLibrary::RandomFloatInRange(0, OutputsTotalSum);
	float CurrentWeight = 0;

	//Resolve the RNG and weights
	for(auto& CurrentOutput : Outputs)
	{
		if(Outputs.Num() >= 3 && CurrentOutput.OutputPinName == LastOutput.OutputPinName)
		{
			CurrentWeight += CurrentOutput.CurrentWeight;
			continue;
		}
		
		if(RandomValue <= CurrentOutput.CurrentWeight)
		{
			TriggerOutput(CurrentOutput.OutputPinName, true);
			CurrentOutput.CurrentWeight = UKismetMathLibrary::Clamp(CurrentOutput.CurrentWeight - CurrentOutput.WeightLostWhenPicked, 0, CurrentOutput.CurrentWeight);
			FinalOutput = CurrentOutput;
			break;
		}
		else
		{
			RandomValue -= CurrentOutput.CurrentWeight;
		}
	}

	//Add up the GainedPerRoll
	for(auto& CurrentOutput : Outputs)
	{
		if(CurrentOutput.OutputPinName != FinalOutput.OutputPinName)
		{
			CurrentOutput.CurrentWeight += CurrentOutput.WeightGainedPerRoll;
		}
	}

	if(Outputs.Num() >= 3)
	{
		LastOutput = FinalOutput;
	}

	return FinalOutput;
}

void UFN_RandomOutputWithWeights::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	RefreshOutputs();

	//Tiny runtime optimization, this will pre-set the CurrentWeight
	//to the base weight so it doesn't have to be set during runtime.
	for(auto& CurrentOutput : Outputs)
	{
		CurrentOutput.CurrentWeight = CurrentOutput.BaseWeight;
	}
	
	OnReconstructionRequested.ExecuteIfBound();
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UFN_RandomOutputWithWeights::RefreshOutputs()
{
	OutputPins.Empty();

	// UpdateCustomOutputPins();

	if (!Outputs.Num())
		OutputPins.Add(TEXT("Continue"));

	for (auto& OutputPin : Outputs)
	{
		OutputPins.Add(FFlowPin(OutputPin.OutputPinName.ToString(), FText(), FString()));
	}
}

void UFN_RandomOutputWithWeights::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	if (PinName == TEXT("In"))
	{
		ExecuteRandomOutput();
		return;
	}

	if (PinName == TEXT("Reset Weights"))
	{
		for(auto& CurrentOutput : Outputs)
		{
			CurrentOutput.CurrentWeight = CurrentOutput.BaseWeight;
		}
	}
}
