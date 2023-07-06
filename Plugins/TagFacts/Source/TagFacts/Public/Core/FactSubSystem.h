// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/CoreTagFactData.h"
#include "FactSubSystem.generated.h"

/**
 * 
 */
UCLASS()
class TAGFACTS_API UFactSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	
	UPROPERTY(SaveGame)
	TSet<FS_Fact> Facts;

public:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/**The fact array is private because you are supposed to interact
	 * with it through the helper functions.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable, BlueprintPure)
	TSet<FS_Fact> GetFacts();

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
	void IncrementFact(FGameplayTag Fact, int32 Amount = 1);

	/**Decrement a fact by one.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable)
	void DecrementFact(FGameplayTag Fact, int32 Amount = 1);

	/**Not advised to be used. This will forcibly set the fact value.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable)
	bool OverrideFactValue(FS_Fact Fact);

	/**Check the fact array and find out if the tag can be found.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable, BlueprintPure)
	bool DoesFactExist(FGameplayTag Fact);

	/**Get the value of a fact. Will return 0 if the fact is not found.
	 * If you need to find out if a fact has a value, use DoesFactExist instead.*/
	UFUNCTION(Category = "Fact System", BlueprintCallable, BlueprintPure)
	int32 GetFactValue(FGameplayTag Fact);
};
