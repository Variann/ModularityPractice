// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "O_TagRelationship.generated.h"


/**This class allows you to create complex relationships between a tag
 * and other tags. Using the AddTagsToObject(Map), it'll automatically
 * try to manage the relationship a tag might have with other tags.*/
UCLASS(Blueprintable)
class OBJECTTAGS_API UO_TagRelationship : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "Base Tag", BlueprintReadOnly, EditAnywhere)
	FGameplayTag Tag;

	/**What tags does the object have to have for the base tag to be applied?*/
	UPROPERTY(Category = "Tag Mapping", BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer RequiredTags;

	/**If any of these tags are found on the object,
	 * the whole operation will be cancelled.*/
	UPROPERTY(Category = "Tag Mapping", BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer BlockingTags;

	/**If true, the base tag will be removed if any of the blocking tags are
	 * applied to the object.
	 * If false, we will initially block the base tag if any of the blocking
	 * tags are present, but will not remove the base tag if any of the
	 * blocking tags are applied after the base tag.*/
	UPROPERTY(Category = "Tag Mapping", BlueprintReadOnly, EditAnywhere)
	bool RemoveTagIfAnyBlockingTagIsApplied = true;

	/**If the base tag is applied successfully, these tags will be removed
	 * from the object.*/
	UPROPERTY(Category = "Tag Mapping", BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer RemoveTags;
};
