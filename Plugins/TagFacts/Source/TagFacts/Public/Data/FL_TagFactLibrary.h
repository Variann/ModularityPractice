// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreTagFactData.h"
#include "Core/GI_FactGameInstance.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FL_TagFactLibrary.generated.h"



UCLASS()
class TAGFACTS_API UFL_TagFactLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(Category = "TagFacts", BlueprintCallable, BlueprintPure, meta=(WorldContext="WorldContext"))
	static UGI_FactGameInstance* GetFactGameInstance(UObject* WorldContext);

	UFUNCTION(Category = "TagFacts|Comparitors", BlueprintCallable, BlueprintPure)
	static bool CompareFact(FS_Fact Fact, int32 Value, TEnumAsByte<EFactComparator> Comparator);
};
