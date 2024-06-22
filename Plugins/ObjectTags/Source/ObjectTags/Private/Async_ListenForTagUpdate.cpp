// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Async_ListenForTagUpdate.h"

#include "ObjectTags_Subsystem.h"

UAsync_ListenForTagUpdate* UAsync_ListenForTagUpdate::ListenForTagUpdate(UObject* Object, FGameplayTag Tag)
{
	UWorld* World = GEngine->GetWorldFromContextObject(Object, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	UAsync_ListenForTagUpdate* Action = NewObject<UAsync_ListenForTagUpdate>();
	Action->ObjectToTrack = Object;
	Action->TagToTrack = Tag;
	Action->RegisterWithGameInstance(World);

	return Action;
}

void UAsync_ListenForTagUpdate::Activate()
{
	UObjectTags_Subsystem::AddListenerToObject(this, ObjectToTrack);
}

void UAsync_ListenForTagUpdate::Cancel()
{
	UObjectTags_Subsystem::RemoveListenerForObject(this, ObjectToTrack);
}

void UAsync_ListenForTagUpdate::ListeningObjectTagsUpdated_Implementation(FGameplayTag Tag,
                                                                          ETagModification Modification, UObject* Object, UObject* Modifier, float NewValue, float Duration)
{
	if(Tag == TagToTrack)
	{
		OnTagUpdate.Broadcast(Tag, Modification, Object, Modifier, NewValue, Duration);
	}
}
