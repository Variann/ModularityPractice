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