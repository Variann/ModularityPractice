// Copyright (C) Varian Daemon 2023. All Rights Reserved.

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

#if WITH_EDITOR

	//Only used to "undo" a change if we add
	//the same tag twice in an array.
	//If a better way is found, this can be removed.
	FGameplayTag OldTag;

#endif

	bool operator==(const FTagMetadata& Argument) const
	{
		return Tag == Argument.Tag;
	}

	bool operator!=(const FTagMetadata& Argument) const
	{
		return Tag != Argument.Tag;
	}

	bool operator==(const FGameplayTag& Argument) const
	{
		return Tag == Argument;
	}

	bool operator!=(const FGameplayTag& Argument) const
	{
		return Tag != Argument;
	}
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

	//TODO V: Add #if WITH_EDITOR. It was removed because for some reason it was not working for the CPP file when packaging

	//Used by the editor utility widget to modify class defaults.
	UFUNCTION(Category = "TagsMetadata|Editor", BlueprintCallable)
	static UObject* GetClassDefaultObject(TSubclassOf<UObject> Class);
	
	
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
};
