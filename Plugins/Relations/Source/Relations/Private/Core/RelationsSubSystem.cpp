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
	float OldExperience = 0;
	float NewExperience = 0;
	if(!Entity->ExperienceAndLevelCurve.GetRichCurve()->Keys.IsValidIndex(0))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Level curve has no keys. Can't add relationship."));
		return false;
	}
	
	//Find the relationship and update it.
	if(FS_Relationship* FoundRelationship = Relationships.Find(FS_Relationship({Entity})))
	{
		OldExperience = FoundRelationship->CurrentXP;
		FoundRelationship->CurrentXP = UKismetMathLibrary::Clamp(FoundRelationship->CurrentXP + Experience, Entity->GetMinimumExperience(), Entity->GetMaximumExperience());
		NewExperience = FoundRelationship->CurrentXP;
	}
	else
	{
		FS_Relationship NewRelationship;
		NewRelationship.Entity = Entity;
		NewRelationship.CurrentXP = Entity->DefaultExperience;
		NewRelationship.CurrentXP = NewRelationship.CurrentXP + Experience;
		NewExperience = NewRelationship.CurrentXP;
		Relationships.Add(NewRelationship);
	}

	//Might be called from a separate thread, make sure to broadcast on game thread so blueprint VM stays happy.
	AsyncTask(ENamedThreads::GameThread, [this, Entity, NewExperience, OldExperience]()
	{
		EntityExperienceUpdated.Broadcast(Entity, NewExperience, OldExperience);
	});
	
	return true;
}

FS_Relationship URelationsSubSystem::GetRelationshipForEntity(UDA_RelationData* Entity)
{
	if(const FS_Relationship* FoundRelationship = Relationships.Find(FS_Relationship({Entity})))
	{
		return *FoundRelationship;
	}

	return FS_Relationship();
}