#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CoreRelationsData.generated.h"

class UDA_RelationData;

USTRUCT(BlueprintType)
struct FS_RelationStatus
{
	GENERATED_BODY()

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	FText StatusText;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	FGameplayTag StatusID;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	int32 MinLevel = 0;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	int32 MaxLevel = 0;
};

USTRUCT(BlueprintType)
struct FS_Relationship
{
	GENERATED_BODY()

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	UDA_RelationData* Entity;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	float CurrentXP;

	UPROPERTY(Category = "Relation", EditAnywhere, BlueprintReadWrite)
	FS_RelationStatus CurrentRelationStatus;

	bool operator==(const FS_Relationship& Argument) const
	{
		return Argument.Entity == Entity;
	}
};
FORCEINLINE uint32 GetTypeHash(const FS_Relationship& Thing)
{
	uint32 Hash = FCrc::MemCrc32(&Thing, sizeof(FS_Relationship));
	return Hash;
}