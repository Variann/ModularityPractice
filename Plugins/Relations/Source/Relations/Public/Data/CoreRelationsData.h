#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AttributeSet.h"
#include "ScalableFloat.h"
#include "CoreRelationsData.generated.h"

USTRUCT(BlueprintType)
struct FS_RelationStatus
{
	GENERATED_BODY()

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	FText StatusText;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	FGameplayTag StatusID;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	float MinLevel;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	float MaxLevel;
};

USTRUCT(BlueprintType)
struct FS_RelationshipsDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	FGameplayTag Entity;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	FText EntityName;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	FRuntimeFloatCurve ExperienceAndLevelCurve;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "StatusText"))
	TArray<FS_RelationStatus> RelationStatuses;
};

USTRUCT(BlueprintType)
struct FS_Relationship
{
	GENERATED_BODY()
	
	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	FGameplayTag Entity;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	float CurrentXP;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	FS_RelationStatus CurrentRelationStatus;
};