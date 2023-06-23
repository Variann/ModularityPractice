// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/CoreTagFactData.h"
#include "Engine/GameInstance.h"
#include "GI_FactGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TAGFACTS_API UGI_FactGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	
	UPROPERTY(SaveGame)
	TArray<FS_Fact> Facts;

public:

	/**The fact array is private because you are supposed to interact
	 * with it through the helper functions.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable, BlueprintPure)
	TArray<FS_Fact> GetFacts();

	/**Add a fact to the system. This will return false if the fact
	 * did not already exist. Will only return true if this function
	 * call added it to the array.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable)
	bool AddFact(FS_Fact Fact);

	/**Remove a fact from the system. Will only return true if the fact
	 * was successfully removed from the system.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable)
	bool RemoveFact(FGameplayTag Fact);

	/**Increment a fact by one.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable)
	void IncrementFact(FGameplayTag Fact);

	/**Decrement a fact by one.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable)
	void DecrementFact(FGameplayTag Fact);

	/**Not advised to be used. This will forcibly set the fact value.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable)
	bool OverrideFactValue(FS_Fact Fact);

	/**Check the fact array and find out if the tag can be found.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable, BlueprintPure)
	bool DoesFactExist(FGameplayTag Fact);

	/**Get the value of a fact. Will return -1 if the fact is not found.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable, BlueprintPure)
	int32 GetFactValue(FGameplayTag Fact);

	/**Get the array index of the fact from the facts array.
	 * Will return -1 if not found.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable, BlueprintPure)
	int32 GetFactIndex(FGameplayTag Fact);
};
