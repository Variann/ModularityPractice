// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/DA_RelationData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RelationsSubSystem.generated.h"



UCLASS()
class RELATIONS_API URelationsSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(SaveGame, BlueprintReadOnly)
	TSet<FS_Relationship> Relationships;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	  //*********************************************************//
	 // Async functions can be found in RelationsAsyncFunctions //
	//*********************************************************//

	/**Add experience for an entity.
	 * If @Async is true, this will be sent to its own thread, but won't be instant.
	 * If you need to add experience and instantly know the new experience, uncheck @Async*/
	UFUNCTION(Category="Relations", BlueprintCallable)
	void AddExperienceToEntity(UDA_RelationData* Entity, float Experience, bool Async = true);

	bool AddExperienceToEntity_Internal(UDA_RelationData* Entity, float Experience);
	
	UFUNCTION(Category="Relations", BlueprintCallable)
	FS_Relationship GetRelationshipForEntity(UDA_RelationData* Entity, bool Async = true);

	bool GetRelationshipForEntity_Internal(UDA_RelationData* Entity, FS_Relationship& Relationship);
};
