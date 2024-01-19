// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "O_TagMetadata.h"
#include "Engine/DeveloperSettings.h"
#include "TagMetadata/Public/O_TagMetadataCollection.h"
#include "DS_TagMetadata.generated.h"

/**
 * Metadata system to extend the usage of GameplayTags.
 * 
 * This config is a place to store both functions and host the array
 * of collections, which store the actual metadata.
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "TagMetadata Settings"))
class TAGMETADATA_API UDS_TagMetadata : public UDeveloperSettings
{
	GENERATED_BODY()

	UDS_TagMetadata();

public:

	/**Collections of tag metadata, these are your hosts of the instanced
	 * objects which grant the tag its metadata.*/
	UPROPERTY(Config, Category = "Settings", BlueprintReadOnly, EditAnywhere)
	TArray<TSubclassOf<UO_TagMetadataCollection>> TagMetadataCollections;

	/**Get all metadata associated with the @Tag.
	 * If @OptionalCollection is filled, the search will be filtered
	 * to just that collection, allowing you to specify which collection
	 * you want the metadata from.*/
	UFUNCTION(Category = "Tags Metadata", BlueprintCallable, BlueprintPure)
	static TArray<UO_TagMetadata*> GetTagMetadata(FGameplayTag Tag, TSubclassOf<UO_TagMetadataCollection> OptionalCollection = nullptr);

	/**Get the specified @Class metadata from the first collection that has any
	 * metadata associated with the @Tag.*/
	UFUNCTION(Category = "Tags Metadata", BlueprintCallable, BlueprintPure, meta = (DeterminesOutputType = "Class"))
	static UO_TagMetadata* GetTagMetadataByClass(FGameplayTag Tag, TSubclassOf<UO_TagMetadata> Class);

	/**Get the specified metadata from a specified collection.*/
	UFUNCTION(Category = "Tags Metadata", BlueprintCallable, BlueprintPure, meta = (DeterminesOutputType = "Class"))
	static UO_TagMetadata* GetTagMetadataByClassFromCollection(FGameplayTag Tag, TSubclassOf<UO_TagMetadata> Class,
		TSubclassOf<UO_TagMetadataCollection> Collection);
};
