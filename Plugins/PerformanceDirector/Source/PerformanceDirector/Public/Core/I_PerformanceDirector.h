// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AC_PerformanceDirector.h"
#include "UObject/Interface.h"
#include "I_PerformanceDirector.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UI_PerformanceDirector : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PERFORMANCEDIRECTOR_API II_PerformanceDirector
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(Category = "Performance Director", BlueprintCallable, BlueprintImplementableEvent)
	UAC_PerformanceDirector* GetPerformanceDirector();

	/**Figure out the current importance of this actor.*/
	UFUNCTION(Category = "Performance Director", BlueprintCallable, BlueprintImplementableEvent)
	TEnumAsByte<EPerformanceImportance> EvaluateImportance();

	UFUNCTION(Category = "Performance Director", BlueprintCallable, BlueprintImplementableEvent)
	void ImportanceUpdated();
};
