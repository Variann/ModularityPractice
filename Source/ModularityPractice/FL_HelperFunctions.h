// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Nodes/FlowNode.h"
#include "FL_HelperFunctions.generated.h"

/**
 * 
 */
UCLASS()
class MODULARITYPRACTICE_API UFL_HelperFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static UObject* GetObjectsDefaultObject(UObject* Object);

	UFUNCTION(BlueprintCallable)
	static TArray<UFlowNode*> GetFlowNodes(UFlowAsset* FlowAsset);
};
