﻿// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/RelationsAsyncFunctions.h"

#include "Core/RelationsSubSystem.h"
#include "Data/DA_RelationData.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"

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

	Handle = StreamableManager.RequestAsyncLoad(EntityToLoad.ToSoftObjectPath(), [this]()
	{
		LoadedEntity = Cast<UDA_RelationData>(Handle->GetLoadedAsset());
		
		//Send job to any background thread
		AsyncTask(ENamedThreads::BackgroundThreadPriority, [this]()
		{
			bool JobSuccessful = false;
			if(LoadedEntity)
			{
				UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetOuter());
				if(!GameInstance)
				{
					return;
				}
			
				URelationsSubSystem* RelationsSubSystem = GameInstance->GetSubsystem<URelationsSubSystem>();
				if(!RelationsSubSystem)
				{
					return;
				}

				if(RelationsSubSystem->AddExperienceToEntity_Internal(LoadedEntity, ExperienceToGrant))
				{
					JobSuccessful = true;
				}
			}

			//Job done, go back to the game thread
			AsyncTask(ENamedThreads::GameThread, [JobSuccessful, this]()
			{
				if(JobSuccessful)
				{
					Success.Broadcast();
				}
				else
				{
					Fail.Broadcast();
				}
				
				RemoveFromRoot();
			});
		});
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

	Handle = StreamableManager.RequestAsyncLoad(EntityToLoad.ToSoftObjectPath(), [this]()
	{
		LoadedEntity = Cast<UDA_RelationData>(Handle->GetLoadedAsset());

		//Send job to any background thread
		AsyncTask(ENamedThreads::BackgroundThreadPriority, [this]()
		{
			bool JobSuccessful = false;
			FS_Relationship Relationship;
			if(LoadedEntity)
			{
				UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetOuter());
				if(!GameInstance)
				{
					//Blueprints might be listening to Fail, make sure to broadcast on game thread.
					AsyncTask(ENamedThreads::GameThread, [this, Relationship]()
					{
						NotFound.Broadcast(Relationship);
						RemoveFromRoot();
					});
			
					return;
				}
			
				URelationsSubSystem* RelationsSubSystem = GameInstance->GetSubsystem<URelationsSubSystem>();
				if(!RelationsSubSystem)
				{
					//Blueprints might be listening to Fail, make sure to broadcast on game thread.
					AsyncTask(ENamedThreads::GameThread, [this, Relationship]()
					{
						NotFound.Broadcast(Relationship);
						RemoveFromRoot();
					});
					return;
				}

				Relationship = RelationsSubSystem->GetRelationshipForEntity(LoadedEntity);
				if(Relationship.Entity)
				{
					JobSuccessful = true;
				}
			}

			//Job done, go back to the game thread
			AsyncTask(ENamedThreads::GameThread, [this, Relationship, JobSuccessful]()
			{
				if(JobSuccessful)
				{
					Found.Broadcast(Relationship);
				}
				else
				{
					NotFound.Broadcast(Relationship);
				}
				
				RemoveFromRoot();
			});
		});
	});
}
