// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/CoreRelationsData.h"
#include "Engine/StreamableManager.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "RelationsAsyncFunctions.generated.h"


class UDA_RelationData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFail);


UCLASS()
class RELATIONS_API URelations_AddExperience : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FSuccess Success;

	UPROPERTY(BlueprintAssignable)
	FFail Fail;

	UPROPERTY()
	TSoftObjectPtr<UDA_RelationData> EntityToLoad;
	UPROPERTY()
	UDA_RelationData* LoadedEntity;

	UPROPERTY()
	float ExperienceToGrant;

	//Needed for loading soft reference.
	FStreamableManager StreamableManager;
	TSharedPtr<FStreamableHandle> Handle;

public:

	/**Add experience for a specified entity.
	 * If the entity is not found, it'll get added.*/
	UFUNCTION(Category="Relations", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context"))
	static URelations_AddExperience* AddExperienceForEntity(TSoftObjectPtr<UDA_RelationData> Entity, float Experience, UObject* Context);

	virtual void Activate() override;
};

UCLASS()
class RELATIONS_API URelations_GetRelationship : public UBlueprintAsyncActionBase
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FoundRelationship, FS_Relationship, Relationship);
	
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FoundRelationship Found;

	UPROPERTY(BlueprintAssignable)
	FoundRelationship NotFound;

	UPROPERTY()
	TSoftObjectPtr<UDA_RelationData> EntityToLoad;
	UPROPERTY()
	UDA_RelationData* LoadedEntity;

	FStreamableManager StreamableManager;
	TSharedPtr<FStreamableHandle> Handle;

public:

	/**Add experience for a specified entity.
	 * If the entity is not found, it'll get added.*/
	UFUNCTION(Category="Relations", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context"))
	static URelations_GetRelationship* GetRelationshipForEntity(TSoftObjectPtr<UDA_RelationData> Entity, UObject* Context);

	virtual void Activate() override;
};
