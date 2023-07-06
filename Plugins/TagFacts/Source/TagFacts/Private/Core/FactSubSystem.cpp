// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\Public\Core\FactSubSystem.h"

#include "Kismet/KismetMathLibrary.h"

bool UFactSubSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

TSet<FS_Fact> UFactSubSystem::GetFacts()
{
	return Facts;
}

bool UFactSubSystem::AddFact(FS_Fact Fact)
{
	if(!Facts.Contains(Fact))
	{
		Facts.Add(Fact);
		return true;
	}

	return false;
}

bool UFactSubSystem::RemoveFact(FGameplayTag Fact)
{
	if(!Facts.IsEmpty())
	{
		Facts.Remove(FS_Fact({Fact}));
		return true;
	}

	return false;
}

void UFactSubSystem::IncrementFact(FGameplayTag Fact, int32 Amount)
{
	if(Amount == 0)
	{
		return;
	}
	
	if(!Facts.IsEmpty())
	{
		FS_Fact* FoundFact = Facts.Find(FS_Fact({Fact}));
		if(FoundFact)
		{
			FoundFact->Value = FoundFact->Value + Amount;
		}
	}
}

void UFactSubSystem::DecrementFact(FGameplayTag Fact, int32 Amount)
{
	if(Amount == 0)
	{
		return;
	}
	
	if(!Facts.IsEmpty())
	{
		FS_Fact* FoundFact = Facts.Find(FS_Fact({Fact}));
		if(FoundFact)
		{
			FoundFact->Value = FoundFact->Value - Amount;
		}
	}
}

bool UFactSubSystem::OverrideFactValue(FS_Fact Fact)
{
	if(!Facts.IsEmpty())
	{
		FS_Fact* FoundFact = Facts.Find(FS_Fact({Fact}));
		if(FoundFact)
		{
			FoundFact->Value = Fact.Value;
		}
	}

	return false;
}

bool UFactSubSystem::DoesFactExist(FGameplayTag Fact)
{
	return Facts.Contains(FS_Fact({Fact}));
}

int32 UFactSubSystem::GetFactValue(FGameplayTag Fact)
{
	if(!Facts.IsEmpty())
	{
		FS_Fact* FoundFact = Facts.Find(FS_Fact({Fact}));
		if(FoundFact)
		{
			return FoundFact->Value;
		}
	}

	return 0;
}
