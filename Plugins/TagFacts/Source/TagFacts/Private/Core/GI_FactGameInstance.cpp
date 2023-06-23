// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GI_FactGameInstance.h"

#include "Kismet/KismetMathLibrary.h"

TArray<FS_Fact> UGI_FactGameInstance::GetFacts()
{
	return Facts;
}

bool UGI_FactGameInstance::AddFact(FS_Fact Fact)
{
	if(!DoesFactExist(Fact.Tag))
	{
		Facts.Add(Fact);
		return true;
	}

	return false;
}

bool UGI_FactGameInstance::RemoveFact(FGameplayTag Fact)
{
	int32 FactIndex = GetFactIndex(Fact);

	if(Facts.IsValidIndex(FactIndex))
	{
		Facts.RemoveAt(FactIndex);
		return true;
	}

	return false;
}

void UGI_FactGameInstance::IncrementFact(FGameplayTag Fact)
{
	int32 FactIndex = GetFactIndex(Fact);

	if(Facts.IsValidIndex(FactIndex))
	{
		Facts[FactIndex].Value = Facts[FactIndex].Value + 1;
	}
}

void UGI_FactGameInstance::DecrementFact(FGameplayTag Fact)
{
	int32 FactIndex = GetFactIndex(Fact);

	if(Facts.IsValidIndex(FactIndex))
	{
		Facts[FactIndex].Value = UKismetMathLibrary::Clamp(Facts[FactIndex].Value - 1, 0, Facts[FactIndex].Value);
	}
}

bool UGI_FactGameInstance::OverrideFactValue(FS_Fact Fact)
{
	int32 FactIndex = GetFactIndex(Fact.Tag);

	if(Facts.IsValidIndex(FactIndex))
	{
		Facts[FactIndex].Value = Fact.Value;
		return true;
	}

	return false;
}

bool UGI_FactGameInstance::DoesFactExist(FGameplayTag Fact)
{
	for(auto& CurrentFact : Facts)
	{
		if(CurrentFact.Tag == Fact)
		{
			return true;
		}
	}

	return false;
}

int32 UGI_FactGameInstance::GetFactValue(FGameplayTag Fact)
{
	for(auto& CurrentFact : Facts)
	{
		if(CurrentFact.Tag == Fact)
		{
			return CurrentFact.Value;
		}
	}

	return -1;
}

int32 UGI_FactGameInstance::GetFactIndex(FGameplayTag Fact)
{
	for(int32 CurrentIndex = 0; CurrentIndex < Facts.Num(); CurrentIndex++)
	{
		if(Facts[CurrentIndex].Tag == Fact)
		{
			return CurrentIndex;
		}
	}

	return -1;
}
