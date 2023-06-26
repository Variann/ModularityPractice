// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/FE_CommonData.h"
#include "Nodes/FlowNode.h"
#include "FN_QuestBase.generated.h"



UCLASS()
class FLOWEXTENSION_API UFN_QuestBase : public UFlowNode
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(EditAnywhere, Category=Dialogue)
	FS_Quest QuestInformation;
};