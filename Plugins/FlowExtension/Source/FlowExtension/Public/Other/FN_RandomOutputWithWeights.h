// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "FN_RandomOutputWithWeights.generated.h"

USTRUCT(BlueprintType)
struct FRandomOutputWithWeight
{
	GENERATED_BODY()

	UPROPERTY(Category = "Randomness", EditAnywhere, BlueprintReadWrite)
	FName OutputPinName;
	
	UPROPERTY(Category = "Randomness", EditAnywhere, BlueprintReadWrite)
	float BaseWeight = 1;

	/**How much is @BaseWeight reduced by when this
	 * option is picked?*/
	UPROPERTY(Category = "Randomness", EditAnywhere, BlueprintReadWrite)
	float WeightLostWhenPicked = 0;

	/**How much does the @BaseWeight get increased each
	 * time the chance is rolled?
	 * This is not added to the output that was picked.*/
	UPROPERTY(Category = "Randomness", EditAnywhere, BlueprintReadWrite)
	float WeightGainedPerRoll = 0;

	UPROPERTY(Category = "Randomness", BlueprintReadWrite)
	float CurrentWeight = 1;
};

UCLASS(DisplayName = "Random output with weights")
class FLOWEXTENSION_API UFN_RandomOutputWithWeights : public UFlowNode
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(Category = "Randomness", EditAnywhere, BlueprintReadWrite)
	TArray<FRandomOutputWithWeight> Outputs;

	/**Last chosen output, this is used to prevent the same output from being
	 * chosen twice in a row.
	 * Only kept track of if the outputs has 3 or more options.*/
	UPROPERTY(Category = "Randomness", BlueprintReadWrite)
	FRandomOutputWithWeight LastOutput;

	UFUNCTION(Category = "Randomness", BlueprintCallable, meta = (ReturnDisplayName = "Chosen output"))
	FRandomOutputWithWeight ExecuteRandomOutput();

	
#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:

	void RefreshOutputs();

#endif

protected:
	virtual void ExecuteInput(const FName& PinName) override;
};
