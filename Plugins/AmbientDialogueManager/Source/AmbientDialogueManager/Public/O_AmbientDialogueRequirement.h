// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "O_AmbientDialogueRequirement.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, AutoExpandCategories = ("Default"), EditInlineNew)
class AMBIENTDIALOGUEMANAGER_API UO_AmbientDialogueRequirement : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(Category = "Dialogue", BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	bool IsConditionMet(AActor* OwningActor);
};
