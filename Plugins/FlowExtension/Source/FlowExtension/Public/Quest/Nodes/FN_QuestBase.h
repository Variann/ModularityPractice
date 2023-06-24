// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "FN_QuestBase.generated.h"



UCLASS()
class FLOWEXTENSION_API UFN_QuestBase : public UFlowNode
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(EditAnywhere, Category=Dialogue)
	FGameplayTag Quest;

	UPROPERTY(EditAnywhere, Category=Dialogue)
	FGameplayTagContainer Tasks;

	UPROPERTY(EditAnywhere, Category=Dialogue)
	FText QuestText;
};