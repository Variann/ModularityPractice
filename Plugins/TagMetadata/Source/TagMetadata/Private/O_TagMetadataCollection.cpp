// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "TagMetadata/Public/O_TagMetadataCollection.h"

#if WITH_EDITOR
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#endif

UObject* UO_TagMetadataCollection::GetClassDefaultObject(TSubclassOf<UObject> Class)
{
	if(Class)
	{
		return Class.GetDefaultObject();
	}

	return nullptr;
}

#if WITH_EDITOR

void UO_TagMetadataCollection::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(FTagMetadata, Tag))
	{
		int32 TagIndex = PropertyChangedEvent.GetArrayIndex(PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue()->GetFName().ToString());
		if(TagIndex == INDEX_NONE)
		{
			return;
		}
		const FGameplayTag NewTag = TagsMetadata[TagIndex].Tag;
		if(!NewTag.IsValid())
		{
			return;
		}

		bool UpdateOldTag = true;
		for(int32 CurrentIndex = 0; CurrentIndex < TagsMetadata.Num(); CurrentIndex++)
		{
			if(TagIndex != CurrentIndex && NewTag == TagsMetadata[CurrentIndex].Tag)
			{
				TagsMetadata[TagIndex].Tag = TagsMetadata[TagIndex].OldTag;
				UpdateOldTag = false;
				FText InfoText = FText::FromString("Collection already has tag.");
				FNotificationInfo Info(InfoText);
				Info.bFireAndForget = true;
				Info.bUseThrobber = false;
				Info.FadeOutDuration = 0.5f;
				Info.ExpireDuration = 5.0f;
				if (TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info))
				{
					Notification->SetCompletionState(SNotificationItem::CS_Fail);
				}
				break;
			}
		}

		if(UpdateOldTag)
		{
			TagsMetadata[TagIndex].OldTag = NewTag;
		}
	}
}
#endif