// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CoreRelationsData.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DA_RelationData.generated.h"

/**
 * 
 */
UCLASS()
class RELATIONS_API UDA_RelationData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	FGameplayTag Entity;

	/**Name of the entity presented to the player.*/
	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	FText EntityName;
	
	/**The default experience granted when this relationship starts.*/
	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	float DefaultExperience;

	/**The levels and experiences required to reach those levels.*/
	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	FRuntimeFloatCurve ExperienceAndLevelCurve;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "StatusText - {MinLevel}/{MaxLevel}"))
	TArray<FS_RelationStatus> RelationStatuses;

	UFUNCTION(Category = "Relation", BlueprintCallable, BlueprintPure)
	FS_RelationStatus GetCurrentStatus(const int32 Level);

	UFUNCTION(Category = "Relation", BlueprintCallable, BlueprintPure)
	int32 GetLevelFromExperience(const float Experience);

	UFUNCTION(Category = "Relation", BlueprintCallable, BlueprintPure)
	float GetMinimumExperience();

	UFUNCTION(Category = "Relation", BlueprintCallable, BlueprintPure)
	float GetMaximumExperience();
};
