// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/Nodes/FlowGraphNode.h"

#include "FGN_Dialogue.generated.h"


UCLASS()
class FLOWEXTENSIONEDITOR_API UFGN_Dialogue : public UFlowGraphNode
{
	GENERATED_UCLASS_BODY()
	
public:
	
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
};
