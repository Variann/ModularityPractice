// Copyright (C) Varian Daemon. All Rights Reserved


#include "ObjectTags_Subsystem.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "I_ObjectTagsCommunication.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UObjectTags_Subsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ObjectTags.Empty();

	FCoreUObjectDelegates::GetPreGarbageCollectDelegate().AddUObject(this, &UObjectTags_Subsystem::CleanupObjectTagArray);
}

FObjectTag UObjectTags_Subsystem::GetObjectTags(UObject* Object)
{
	if(!UGameplayStatics::GetPlayerController(Object, 0))
	{
		return FObjectTag();
	}
	
	UObjectTags_Subsystem* ObjectTags_Subsystem = UGameplayStatics::GetPlayerController(Object, 0)->GetLocalPlayer()->GetSubsystem<UObjectTags_Subsystem>();
	if(!ObjectTags_Subsystem)
	{
		return FObjectTag();
	}

	return ObjectTags_Subsystem->GetObjectTags_Internal(Object);
}

FObjectTag UObjectTags_Subsystem::GetObjectTags_Internal(UObject* Object)
{
	const int32 ObjectIndex = ObjectTags.Find(FObjectTag({Object}));
	FObjectTag FoundTags = FObjectTag();
	if(ObjectTags.IsValidIndex(ObjectIndex) && ObjectTags[ObjectIndex].Object)
	{
		FoundTags = ObjectTags[ObjectIndex];
	}

	if(UKismetSystemLibrary::DoesImplementInterface(Object, UI_ObjectTagsCommunication::StaticClass()))
	{
		const FGameplayTagContainer ExternalTags = II_ObjectTagsCommunication::Execute_GetObjectsExternalTags(Object);
		if(ExternalTags.IsValid())
		{
			FoundTags.Tags.AppendTags(ExternalTags);
		}
	}

	return FoundTags;
}

bool UObjectTags_Subsystem::AddTagsToObject(FGameplayTagContainer TagsToAdd, UObject* Object, UObject* Modifier)
{
	if(!UGameplayStatics::GetPlayerController(Object, 0))
	{
		return false;
	}
	
	UObjectTags_Subsystem* ObjectTags_Subsystem = UGameplayStatics::GetPlayerController(Object, 0)->GetLocalPlayer()->GetSubsystem<UObjectTags_Subsystem>();
	if(!ObjectTags_Subsystem)
	{
		return false;
	}
	
	return ObjectTags_Subsystem->AddTagsToObject_Internal(TagsToAdd, Object, Modifier);
}

bool UObjectTags_Subsystem::AddTagsToObject_Internal(FGameplayTagContainer TagsToAdd, UObject* Object,
	UObject* Modifier)
{
	if(!Object)
	{
		return false;
	}
	
	bool bTagAdded = false;
	int32 ObjectIndex = ObjectTags.Find(FObjectTag({Object}));
	if(ObjectTags.IsValidIndex(ObjectIndex) && ObjectTags[ObjectIndex].Object)
	{
		//Append the two tag containers and call the TagsModified delegate on the way. - V
		for(auto& CurrentTag : TagsToAdd)
		{
			if(!ObjectTags[ObjectIndex].Tags.HasTagExact(CurrentTag))
			{
				ObjectTags[ObjectIndex].Tags.AddTag(CurrentTag);

				//Notify the object itself
				if(UKismetSystemLibrary::DoesImplementInterface(Object, UI_ObjectTagsCommunication::StaticClass()))
				{
					II_ObjectTagsCommunication::Execute_OwningTagsUpdated(Object, CurrentTag, true, Modifier);
				}

				if(ObjectTags[ObjectIndex].Listeners.IsValidIndex(0))
				{
					//Notify any listeners
					for(int32 CurrentListener = 0; CurrentListener < ObjectTags[ObjectIndex].Listeners.Num(); CurrentListener++)
					{
						if(!ObjectTags[ObjectIndex].Listeners[CurrentListener])
						{
							continue;
						}
					
						if(UKismetSystemLibrary::DoesImplementInterface(ObjectTags[ObjectIndex].Listeners[CurrentListener], UI_ObjectTagsCommunication::StaticClass()))
						{
							II_ObjectTagsCommunication::Execute_ListeningObjectTagsUpdated(ObjectTags[ObjectIndex].Listeners[CurrentListener], CurrentTag, true, Object, Modifier);
						}
					}
				}

				if(ObjectTags[ObjectIndex].TagRelationships.IsValidIndex(0))
				{
					//Find out if any current relationships would be removed from the application of this tag.
					for(int32 CurrentRelationship = 0; CurrentRelationship < ObjectTags[ObjectIndex].TagRelationships.Num(); CurrentRelationship++)
					{
						UO_TagRelationship* CurrentTagCDO = ObjectTags[ObjectIndex].TagRelationships[CurrentRelationship].GetDefaultObject();
						if(CurrentTagCDO->BlockingTags.HasTagExact(CurrentTag))
						{
							RemoveTagsFromObject_Internal(FGameplayTagContainer({CurrentTagCDO->Tag}), Object, Modifier);
							ObjectTags[ObjectIndex].TagRelationships.RemoveAt(CurrentRelationship);
							CurrentRelationship--;
						}
					}
				}
			
				//For now the success logic is simple, as long as any tag
				//was added, we label this function as a success. - V
				bTagAdded = true;
			}
		}

		//It's simpler to have this system interact with GAS's tag system rather than the other way around.
		if(ObjectTags[ObjectIndex].Object)
		{
			if(AActor* TargetActor = Cast<AActor>(ObjectTags[ObjectIndex].Object.Get()))
			{
				UAbilitySystemBlueprintLibrary::AddLooseGameplayTags(TargetActor, TagsToAdd, true);
			}
		}
	}
	else
	{
		FObjectTag NewObjectTag;
		NewObjectTag.Object = Object;
		for(auto& CurrentTag : TagsToAdd)
		{
			NewObjectTag.Tags.AddTag(CurrentTag);

			//Notify the object itself
			if(UKismetSystemLibrary::DoesImplementInterface(Object, UI_ObjectTagsCommunication::StaticClass()))
			{
				II_ObjectTagsCommunication::Execute_OwningTagsUpdated(Object, CurrentTag, true, Modifier);
			}
		}
		ObjectIndex = ObjectTags.Add(NewObjectTag);
		bTagAdded = true;
	}

#if WITH_EDITOR

	if(bTagAdded && CollectDebuggingData)
	{
		ObjectTags[ObjectIndex].TagHistory.Add(FObjectTagHistory(UKismetSystemLibrary::GetDisplayName(Modifier), TagsToAdd, true));
	}
	
#endif

	return bTagAdded;
}

bool UObjectTags_Subsystem::AddTagToObjectWithRelationship(TSubclassOf<UO_TagRelationship> TagRelationship, UObject* Object,
	UObject* Modifier)
{
	if(!UGameplayStatics::GetPlayerController(Object, 0))
	{
		return false;
	}
	
	UObjectTags_Subsystem* ObjectTags_Subsystem = UGameplayStatics::GetPlayerController(Object, 0)->GetLocalPlayer()->GetSubsystem<UObjectTags_Subsystem>();
	if(!ObjectTags_Subsystem)
	{
		return false;
	}
	
	return ObjectTags_Subsystem->AddTagToObjectWithRelationship_Internal(TagRelationship, Object, Modifier);
}

bool UObjectTags_Subsystem::AddTagToObjectWithRelationship_Internal(TSubclassOf<UO_TagRelationship> TagRelationship, UObject* Object,
	UObject* Modifier)
{
	UO_TagRelationship* TagCDO = TagRelationship.GetDefaultObject();
	if(!TagCDO)
	{
		return false;	
	}

	int32 ObjectIndex = ObjectTags.Find(FObjectTag({Object}));
	if(ObjectTags.IsValidIndex(ObjectIndex) && ObjectTags[ObjectIndex].Object)
	{
			
		//Check if object already has the tag.
		if(ObjectTags[ObjectIndex].Tags.HasTagExact(TagCDO->Tag))
		{
			return false;
		}

		//Check if object has any of the blocking tags.
		if(ObjectTags[ObjectIndex].Tags.HasAnyExact(TagCDO->BlockingTags))
		{
			return false;
		}

		//Check if the object has all of the required tags.
		if(!ObjectTags[ObjectIndex].Tags.HasAllExact(TagCDO->RequiredTags))
		{
			return false;
		}

		//Remove any tags this relationship wants to remove.
		RemoveTagsFromObject_Internal(TagCDO->RemoveTags, Object, Modifier);

		//Object met requirements, add the tag.
		AddTagsToObject_Internal(FGameplayTagContainer({TagCDO->Tag}), Object, Modifier);

		if(TagCDO->RemoveTagIfAnyBlockingTagIsApplied)
		{
			ObjectTags[ObjectIndex].TagRelationships.Add(TagRelationship);
		}

		// //Find out if any current relationships would be removed from the application of this tag.
		// for(auto& CurrentObject : ObjectTags)
		// {
		// 	if(CurrentObject.TagRelationships.IsValidIndex(0))
		// 	{
		// 		for(int32 CurrentRelationship = 0; CurrentRelationship < FoundObject->TagRelationships.Num(); CurrentRelationship++)
		// 		{
		// 			//Fetch the CDO
		// 			UO_TagRelationship* CurrentTagCDO = CurrentObject.TagRelationships[CurrentRelationship].GetDefaultObject();
		// 			if(CurrentTagCDO->BlockingTags.HasTagExact(TagCDO->Tag) && CurrentTagCDO->RemoveTagIfAnyBlockingTagIsApplied)
		// 			{
		// 				RemoveTagsFromObject_Internal(FGameplayTagContainer({CurrentTagCDO->Tag}), Object, Modifier);
		// 				//The relationships tag was removed, remove the relationship
		// 				CurrentObject.TagRelationships.RemoveAt(CurrentRelationship);
		// 				CurrentRelationship--;
		// 			}
		// 		}
		// 	}
		// }


		return true;
	}
	else
	{
		FObjectTag NewObjectTag;
		NewObjectTag.Object = Object;
		NewObjectTag.Tags.AddTag(TagCDO->Tag);

		//Notify the object itself
		if(UKismetSystemLibrary::DoesImplementInterface(Object, UI_ObjectTagsCommunication::StaticClass()))
		{
			II_ObjectTagsCommunication::Execute_OwningTagsUpdated(Object, TagCDO->Tag, true, Modifier);
		}
		ObjectTags.Add(NewObjectTag);
		return true;
	}

	return false;
}

bool UObjectTags_Subsystem::RemoveTagsFromObject(FGameplayTagContainer TagsToRemove, UObject* Object, UObject* Modifier)
{
	if(!UGameplayStatics::GetPlayerController(Object, 0))
	{
		return false;
	}
	
	UObjectTags_Subsystem* ObjectTags_Subsystem = UGameplayStatics::GetPlayerController(Object, 0)->GetLocalPlayer()->GetSubsystem<UObjectTags_Subsystem>();
	if(!ObjectTags_Subsystem)
	{
		return false;
	}

	return ObjectTags_Subsystem->RemoveTagsFromObject_Internal(TagsToRemove, Object, Modifier);
}

bool UObjectTags_Subsystem::RemoveTagsFromObject_Internal(FGameplayTagContainer TagsToRemove, UObject* Object,
	UObject* Modifier)
{
	bool bTagRemoved = false;

	int32 ObjectIndex = ObjectTags.Find(FObjectTag({Object}));
	if(ObjectTags.IsValidIndex(ObjectIndex) && ObjectTags[ObjectIndex].Object)
	{
		for(auto& CurrentTag : TagsToRemove)
		{
			if(ObjectTags[ObjectIndex].Tags.HasTagExact(CurrentTag))
			{
				ObjectTags[ObjectIndex].Tags.RemoveTag(CurrentTag);

				//Notify the object itself
				if(UKismetSystemLibrary::DoesImplementInterface(Object, UI_ObjectTagsCommunication::StaticClass()))
				{
					II_ObjectTagsCommunication::Execute_OwningTagsUpdated(Object, CurrentTag, false, Modifier);
				}

				//Notify any listeners
				for(auto& CurrentListener : ObjectTags[ObjectIndex].Listeners)
				{
					if(UKismetSystemLibrary::DoesImplementInterface(CurrentListener, UI_ObjectTagsCommunication::StaticClass()))
					{
						II_ObjectTagsCommunication::Execute_ListeningObjectTagsUpdated(CurrentListener, CurrentTag, false, Object, Modifier);
					}
				}

				//For now the success logic is simple, as long as any tag
				//was added, we label this function as a success. - V
				bTagRemoved = true;
			}
		}

		//It's simpler to have this system interact with GAS's tag system rather than the other way around.
		if(AActor* TargetActor = Cast<AActor>(ObjectTags[ObjectIndex].Object))
		{
			UAbilitySystemBlueprintLibrary::RemoveLooseGameplayTags(TargetActor, TagsToRemove, true);
		}
	}

#if WITH_EDITOR

	if(bTagRemoved && CollectDebuggingData)
	{
		ObjectTags[ObjectIndex].TagHistory.Add(FObjectTagHistory(UKismetSystemLibrary::GetDisplayName(Modifier), TagsToRemove, false));
	}
	
#endif

	if(!ObjectTags[ObjectIndex].Object.Get() || (ObjectTags[ObjectIndex].Tags.IsEmpty() && ObjectTags[ObjectIndex].Listeners.IsEmpty()))
	{
		ObjectTags.Remove(FObjectTag({Object}));
	}
	
	return bTagRemoved;
}

bool UObjectTags_Subsystem::DoesObjectHaveTag(FGameplayTag Tag, UObject* Object)
{
	if(!UGameplayStatics::GetPlayerController(Object, 0))
	{
		return false;
	}
	
	UObjectTags_Subsystem* ObjectTags_Subsystem = UGameplayStatics::GetPlayerController(Object, 0)->GetLocalPlayer()->GetSubsystem<UObjectTags_Subsystem>();
	if(!ObjectTags_Subsystem)
	{
		return false;
	}
	
	if(const FGameplayTagContainer FoundTags = ObjectTags_Subsystem->GetObjectTags(Object).Tags; FoundTags.HasTagExact(Tag))
	{
		return true;
	}

	return false;
}

bool UObjectTags_Subsystem::AddListenerToObject(UObject* Listener, UObject* OtherObject)
{
	if(!UGameplayStatics::GetPlayerController(OtherObject, 0))
	{
		return false;
	}
	
	UObjectTags_Subsystem* ObjectTags_Subsystem = UGameplayStatics::GetPlayerController(OtherObject, 0)->GetLocalPlayer()->GetSubsystem<UObjectTags_Subsystem>();
	if(!ObjectTags_Subsystem)
	{
		return false;
	}

	return ObjectTags_Subsystem->AddListenerToObject_Internal(Listener, OtherObject);
}

bool UObjectTags_Subsystem::AddListenerToObject_Internal(UObject* Listener, UObject* OtherObject)
{
	bool bListenerAdded = false;

	int32 ObjectIndex = ObjectTags.Find(FObjectTag({OtherObject}));
	if(ObjectTags.IsValidIndex(ObjectIndex) && ObjectTags[ObjectIndex].Object)
	{
		if(!ObjectTags[ObjectIndex].Listeners.Contains(Listener))
		{
			ObjectTags[ObjectIndex].Listeners.Add(Listener);
			bListenerAdded = true;
		}
	}
	else
	{
		FObjectTag NewObjectTag;
		NewObjectTag.Object = OtherObject;
		NewObjectTag.Listeners.AddUnique(Listener);
		bListenerAdded = true;
		ObjectTags.Add(NewObjectTag);
	}

	return bListenerAdded;
}

bool UObjectTags_Subsystem::RemoveListenerForObject(UObject* Listener, UObject* OtherObject)
{
	if(!UGameplayStatics::GetPlayerController(OtherObject, 0))
	{
		return false;
	}
	
	UObjectTags_Subsystem* ObjectTags_Subsystem = UGameplayStatics::GetPlayerController(OtherObject, 0)->GetLocalPlayer()->GetSubsystem<UObjectTags_Subsystem>();
	if(!ObjectTags_Subsystem)
	{
		return false;
	}

	return ObjectTags_Subsystem->RemoveListenerForObject_Internal(Listener, OtherObject);
}

bool UObjectTags_Subsystem::RemoveListenerForObject_Internal(UObject* Listener, UObject* OtherObject)
{
	bool bListenerRemoved = false;
	
	int32 ObjectIndex = ObjectTags.Find(FObjectTag({OtherObject}));
	if(ObjectTags.IsValidIndex(ObjectIndex) && ObjectTags[ObjectIndex].Object)
	{
		if(ObjectTags[ObjectIndex].Listeners.Contains(Listener))
		{
			ObjectTags[ObjectIndex].Listeners.Remove(Listener);
			bListenerRemoved = true;
		}		
	}

	return bListenerRemoved;
}

void UObjectTags_Subsystem::CleanupObjectTagArray()
{
	TArray<FObjectTag> ObjectTagsCopy = ObjectTags;
	for(auto& CurrentObject : ObjectTagsCopy)
	{
		//If an object still has a listener, we do not want to remove the object. Only if a object has no
		//tags and has no listener, then remove it, even though the object is valid.
		if(!CurrentObject.Object.Get() || (CurrentObject.Tags.IsEmpty() && CurrentObject.Listeners.IsEmpty()))
		{
			ObjectTags.Remove(CurrentObject);
		}
	}
}
