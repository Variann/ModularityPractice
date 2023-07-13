// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/RelationsAsyncFunctions.h"

#include "Core/RelationsSubSystem.h"
#include "Data/DA_RelationData.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

URelations_AddExperience* URelations_AddExperience::AddExperienceForEntity(TSoftObjectPtr<UDA_RelationData> Entity, float Experience, UObject* Context)
{
	URelations_AddExperience* NewAsyncObject = NewObject<URelations_AddExperience>(Context);
	NewAsyncObject->EntityToLoad = Entity;
	NewAsyncObject->ExperienceToGrant = Experience;
	return NewAsyncObject;
}

void URelations_AddExperience::Activate()
{
	Super::Activate();

	Handle = StreamableManager.RequestAsyncLoad(EntityToLoad.ToString(), [this]()
	{
		LoadedEntity = Cast<UDA_RelationData>(Handle->GetLoadedAsset());

		if(LoadedEntity)
		{
			UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetOuter());
			if(!GameInstance)
			{
				Fail.Broadcast();
				RemoveFromRoot();
				return;
			}
			
			URelationsSubSystem* RelationsSubSystem = GameInstance->GetSubsystem<URelationsSubSystem>();
			if(!RelationsSubSystem)
			{
				Fail.Broadcast();
				RemoveFromRoot();
				return;
			}

			if(RelationsSubSystem->AddExperienceToEntity_Internal(LoadedEntity, ExperienceToGrant))
			{
				Success.Broadcast();
			}
			else
			{
				Fail.Broadcast();
			}
		}
		else
		{
			Fail.Broadcast();
		}

		RemoveFromRoot();
	});
}

URelations_GetRelationship* URelations_GetRelationship::GetRelationshipForEntity(
	TSoftObjectPtr<UDA_RelationData> Entity, UObject* Context)
{
	URelations_GetRelationship* NewAsyncObject = NewObject<URelations_GetRelationship>(Context);
	NewAsyncObject->EntityToLoad = Entity;
	return NewAsyncObject;
}

void URelations_GetRelationship::Activate()
{
	Super::Activate();

	Handle = StreamableManager.RequestAsyncLoad(EntityToLoad.ToString(), [this]()
	{
		FS_Relationship Relationship;

		LoadedEntity = Cast<UDA_RelationData>(Handle->GetLoadedAsset());
		if(LoadedEntity)
		{
			UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetOuter());
			if(!GameInstance)
			{
				NotFound.Broadcast(Relationship);
				RemoveFromRoot();
				return;
			}
			
			URelationsSubSystem* RelationsSubSystem = GameInstance->GetSubsystem<URelationsSubSystem>();
			if(!RelationsSubSystem)
			{
				NotFound.Broadcast(Relationship);
				RemoveFromRoot();
				return;
			}

			
			if(RelationsSubSystem->GetRelationshipForEntity_Internal(LoadedEntity, Relationship))
			{
				Found.Broadcast(Relationship);
			}
			else
			{
				NotFound.Broadcast(Relationship);
			}
		}
		else
		{
			NotFound.Broadcast(Relationship);
		}

		RemoveFromRoot();
	});
}
