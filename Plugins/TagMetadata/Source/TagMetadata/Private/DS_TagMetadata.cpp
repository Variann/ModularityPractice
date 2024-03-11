// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#include "TagMetadata/Public/DS_TagMetadata.h"

UDS_TagMetadata::UDS_TagMetadata()
{
	SectionName = "TagMetadata Settings";
	CategoryName = "Plugins";
}

TArray<UO_TagMetadata*> UDS_TagMetadata::GetTagMetadata(FGameplayTag Tag, TSubclassOf<UO_TagMetadataCollection> OptionalCollection)
{
	TArray<UO_TagMetadata*> FoundMetadata;

	if(IsValid(OptionalCollection))
	{
		for(auto& CurrentMetadata : Cast<UO_TagMetadataCollection>(OptionalCollection->ClassDefaultObject)->TagsMetadata)
		{
			if(CurrentMetadata.Tag == Tag)
			{
				FoundMetadata.Append(CurrentMetadata.Metadata);
			}
		}

		return FoundMetadata;
	}
	
	if(const UDS_TagMetadata* TagMetadataConfig = GetDefault<UDS_TagMetadata>())
	{
		for(auto& CurrentCollection : TagMetadataConfig->TagMetadataCollections)
		{
			if(IsValid(CurrentCollection))
			{
				for(auto& CurrentMetadata : Cast<UO_TagMetadataCollection>(CurrentCollection->ClassDefaultObject)->TagsMetadata)
				{
					if(CurrentMetadata.Tag == Tag)
					{
						FoundMetadata.Append(CurrentMetadata.Metadata);
					}
				}
			}
		}
	}
	
	return FoundMetadata;
}

UO_TagMetadata* UDS_TagMetadata::GetTagMetadataByClass(FGameplayTag Tag, TSubclassOf<UO_TagMetadata> Class)
{
	for(TArray<UO_TagMetadata*> TagMetadata = GetTagMetadata(Tag); const auto& CurrentMetadata : TagMetadata)
	{
		if(IsValid(CurrentMetadata))
		{
			if(CurrentMetadata->GetClass() == Class)
			{
				return CurrentMetadata;
			}
		}
	}

	return nullptr;
}

UO_TagMetadata* UDS_TagMetadata::GetTagMetadataByClassFromCollection(FGameplayTag Tag,
	TSubclassOf<UO_TagMetadata> Class, TSubclassOf<UO_TagMetadataCollection> Collection)
{
	TArray<UO_TagMetadata*> TagMetadata = GetTagMetadata(Tag, Collection);
	for(const auto& CurrentMetadata : TagMetadata)
	{
		if(IsValid(CurrentMetadata))
		{
			if(CurrentMetadata->GetClass() == Class)
			{
				return CurrentMetadata;
			}
		}
	}

	return nullptr;
}

TArray<TSubclassOf<UO_TagMetadataCollection>> UDS_TagMetadata::GetAllCollectionsForTag(FGameplayTag Tag)
{
	TArray<TSubclassOf<UO_TagMetadataCollection>> FoundCollections;
	
	if(const UDS_TagMetadata* TagMetadataConfig = GetDefault<UDS_TagMetadata>())
	{
		for(auto& CurrentCollection : TagMetadataConfig->TagMetadataCollections)
		{
			for(auto& CurrentTagMetadata : CurrentCollection.GetDefaultObject()->TagsMetadata)
			{
				if(CurrentTagMetadata.Tag == Tag)
				{
					FoundCollections.Add(CurrentCollection);
					break;
				}
			}
		}
	}

	return FoundCollections;
}
