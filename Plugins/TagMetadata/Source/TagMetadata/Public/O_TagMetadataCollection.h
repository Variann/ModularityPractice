// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "O_TagMetadataCollection.generated.h"

class UO_TagMetadata;

USTRUCT(BlueprintType)
struct FTagMetadata
{
	GENERATED_BODY()

	UPROPERTY(Category = "Keywords", BlueprintReadOnly, EditAnywhere)
	FGameplayTag Tag;

	UPROPERTY(Category = "Keywords", BlueprintReadOnly, EditAnywhere, Instanced, meta = (ShowOnlyInnerProperties))
	TArray<UO_TagMetadata*> Metadata;
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class TAGMETADATA_API UO_TagMetadataCollection : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "Tags Metadata", BlueprintReadOnly, EditAnywhere, meta = (TitleProperty = "Tag"))
	TArray<FTagMetadata> TagsMetadata;

	//TODO: Check if a tag is already present in the array
	// virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};
