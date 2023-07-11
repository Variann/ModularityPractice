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

void UFactSubSystem::SetFacts(TSet<FS_Fact> NewFacts)
{
	Facts = NewFacts;
}

bool UFactSubSystem::AddFact(FS_Fact Fact)
{
	if(!Facts.Contains(Fact))
	{
		Facts.Add(Fact);
		FactAdded.Broadcast(Fact);
		
		return true;
	}

	return false;
}

bool UFactSubSystem::RemoveFact(FGameplayTag Fact)
{
	if(!Facts.IsEmpty())
	{
		const FS_Fact* FoundFact = Facts.Find(FS_Fact({Fact}));
		Facts.Remove(FS_Fact({Fact}));
		FactRemoved.Broadcast(FS_Fact({FoundFact->Tag, FoundFact->Value}));
		
		return true;
	}

	return false;
}

void UFactSubSystem::IncrementFact(const FGameplayTag Fact, const int32 Amount)
{
	if(Amount == 0)
	{
		return;
	}
	
	if(!Facts.IsEmpty())
	{
		if(FS_Fact* FoundFact = Facts.Find(FS_Fact({Fact})))
		{
			const int32 OldValue = FoundFact->Value;
			FoundFact->Value = FoundFact->Value + Amount;
			FactIncremented.Broadcast(FS_Fact({FoundFact->Tag, FoundFact->Value}), OldValue);
		}
	}
}

void UFactSubSystem::DecrementFact(const FGameplayTag Fact, const int32 Amount)
{
	if(Amount == 0)
	{
		return;
	}
	
	if(!Facts.IsEmpty())
	{
		if(FS_Fact* FoundFact = Facts.Find(FS_Fact({Fact})))
		{
			const int32 OldValue = FoundFact->Value;
			FoundFact->Value = FoundFact->Value - Amount;
			FactDecremented.Broadcast(FS_Fact({FoundFact->Tag, FoundFact->Value}), OldValue);
		}
	}
}

bool UFactSubSystem::OverrideFactValue(FS_Fact Fact)
{
	if(!Facts.IsEmpty())
	{
		if(FS_Fact* FoundFact = Facts.Find(FS_Fact({Fact})))
		{
			const int32 OldValue = FoundFact->Value;
			const int32 NewValue = Fact.Value;
			FoundFact->Value = Fact.Value;
			FactOverriden.Broadcast(OldValue, NewValue);
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
