// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AC_PerformanceDirector.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FL_PerformanceDirector.generated.h"

/**
 * 
 */
UCLASS()
class PERFORMANCEDIRECTOR_API UFL_PerformanceDirector : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(Category = "Performance Director", BlueprintCallable)
	static TEnumAsByte<EPerformanceImportance> GetActorsImportance(AActor* Actor, bool EvaluateImportance);
};
