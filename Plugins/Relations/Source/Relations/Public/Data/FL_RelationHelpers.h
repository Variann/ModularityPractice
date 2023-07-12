// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CoreRelationsData.h"
#include "ScalableFloat.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FL_RelationHelpers.generated.h"

/**
 * 
 */
UCLASS()
class RELATIONS_API UFL_RelationHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static float GetValueAtLevel(FScalableFloat ScalableFloat, float Level, FString ContextString);

	UFUNCTION(BlueprintCallable)
	static TMap<float, float> GetRuntimeCurveKeys(FRuntimeFloatCurve RuntimeCurveFloat);
};
