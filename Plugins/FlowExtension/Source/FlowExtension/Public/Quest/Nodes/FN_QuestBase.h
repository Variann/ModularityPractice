// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/FE_CommonData.h"
#include "Nodes/FlowNode.h"
#include "FN_QuestBase.generated.h"



UCLASS(AutoExpandCategories = "Quest", Abstract)
class FLOWEXTENSION_API UFN_QuestBase : public UFlowNode
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(Category= "Quest", EditAnywhere)
	FS_Quest QuestInformation;

#if WITH_EDITOR
	virtual EDataValidationResult ValidateNode() override;
#endif
};