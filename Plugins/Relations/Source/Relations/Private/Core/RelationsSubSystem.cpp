// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/RelationsSubSystem.h"

#include "Core/RelationsAsyncFunctions.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

bool URelationsSubSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void URelationsSubSystem::AddExperienceToEntity(UDA_RelationData* Entity, float Experience, bool Async)
{
	if(Async)
	{
		URelations_AddExperience* AddExperience = URelations_AddExperience::AddExperienceForEntity(Entity, Experience, this);
		AddExperience->Activate();
		return;
	}

	AddExperienceToEntity_Internal(Entity, Experience);
}

bool URelationsSubSystem::AddExperienceToEntity_Internal(UDA_RelationData* Entity, float Experience)
{
	if(FS_Relationship* FoundRelationship =Relationships.Find(FS_Relationship({Entity})))
	{
		FoundRelationship->CurrentXP = UKismetMathLibrary::Clamp(FoundRelationship->CurrentXP + Experience, Entity->GetMinimumExperience(), Entity->GetMaximumExperience());
		return true;
	}
	
	if(!Entity->ExperienceAndLevelCurve.GetRichCurve()->Keys.IsValidIndex(0))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Level curve has no keys. Can't add relationship."));
		return false;
	}
		
	FS_Relationship NewRelationship;
	NewRelationship.Entity = Entity;
	NewRelationship.CurrentXP = Entity->DefaultExperience;
	NewRelationship.CurrentXP = NewRelationship.CurrentXP + Experience;
		
	Relationships.Add(NewRelationship);
	return true;
}

FS_Relationship URelationsSubSystem::GetRelationshipForEntity(UDA_RelationData* Entity, bool Async)
{
	FS_Relationship FoundRelationship;
	if(Async)
	{
		URelations_GetRelationship* GetExperience = URelations_GetRelationship::GetRelationshipForEntity(Entity, this);
		GetExperience->Activate();
		return FoundRelationship;
	}

	GetRelationshipForEntity_Internal(Entity, FoundRelationship);
	return FoundRelationship;
}

bool URelationsSubSystem::GetRelationshipForEntity_Internal(UDA_RelationData* Entity, FS_Relationship& Relationship)
{
	if(const FS_Relationship* FoundRelationship = Relationships.Find(FS_Relationship({Entity})))
	{
		Relationship = *FoundRelationship;
		return true;
	}

	Relationship = FS_Relationship();
	return false;
}

