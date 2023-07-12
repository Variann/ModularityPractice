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
	
			if(FS_Relationship* FoundRelationship = RelationsSubSystem->Relationships.Find(FS_Relationship({LoadedEntity})))
			{
				FoundRelationship->CurrentXP = UKismetMathLibrary::Clamp(FoundRelationship->CurrentXP + ExperienceToGrant, LoadedEntity->GetMinimumExperience(), LoadedEntity->GetMaximumExperience());
				Success.Broadcast();
				RemoveFromRoot();
				return;
			}
	
			if(!LoadedEntity->ExperienceAndLevelCurve.GetRichCurve()->Keys.IsValidIndex(0))
			{
				UKismetSystemLibrary::PrintString(this, TEXT("Level curve has no keys. Can't add relationship."));
				Fail.Broadcast();
				RemoveFromRoot();
				return;
			}
		
			FS_Relationship NewRelationship;
			NewRelationship.Entity = LoadedEntity;
			NewRelationship.CurrentXP = LoadedEntity->DefaultExperience;
			NewRelationship.CurrentXP = NewRelationship.CurrentXP + ExperienceToGrant;
		
			RelationsSubSystem->Relationships.Add(NewRelationship);
			Success.Broadcast();
		}
		else
		{
			Fail.Broadcast();
		}

		RemoveFromRoot();
	});
}