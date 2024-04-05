// Copyright (C) Varian Daemon. All Rights Reserved


#include "ObjectTags_Subsystem.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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
	if(FObjectTag* FoundObject = ObjectTags.Find(Object))
	{
		//Make a copy so we can append external tags without modifying the original
		FObjectTag FoundTags = *FoundObject;
		if(UKismetSystemLibrary::DoesImplementInterface(Object, UI_ObjectTagsCommunication::StaticClass()))
		{
			const FGameplayTagContainer ExternalTags = II_ObjectTagsCommunication::Execute_GetObjectsExternalTags(Object);
			if(ExternalTags.IsValid())
			{
				FoundTags.Tags.AppendTags(ExternalTags);
			}
		}

		if(FoundObject->Object)
		{
			if(AActor* TargetActor = Cast<AActor>(Object))
			{
				FGameplayTagContainer ASCTags;
				UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor)->GetOwnedGameplayTags(ASCTags);
				FoundTags.Tags.AppendTags(ASCTags);
			}
		}
		
		return FoundTags;
	}

	return FObjectTag();
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
	FObjectTag* FoundObject = ObjectTags.Find(Object);
	if(FoundObject)
	{
		//Append the two tag containers and call the TagsModified delegate on the way. - V
		for(auto& CurrentTag : TagsToAdd)
		{
			if(!GetObjectTags(FoundObject->Object).Tags.HasTagExact(CurrentTag))
			{
				FoundObject->Tags.AddTag(CurrentTag);

				if(AActor* TargetActor = Cast<AActor>(FoundObject->Object.Get()))
				{
					UAbilitySystemBlueprintLibrary::AddLooseGameplayTags(TargetActor, FGameplayTagContainer({CurrentTag}), true);
				}

				//Notify the object itself
				if(UKismetSystemLibrary::DoesImplementInterface(Object, UI_ObjectTagsCommunication::StaticClass()))
				{
					II_ObjectTagsCommunication::Execute_OwningTagsUpdated(Object, CurrentTag, true, Modifier);
				}

				if(FoundObject->Listeners.IsValidIndex(0))
				{
					//Notify any listeners
					for(int32 CurrentListener = 0; CurrentListener < FoundObject->Listeners.Num(); CurrentListener++)
					{
						if(!FoundObject->Listeners[CurrentListener])
						{
							continue;
						}
					
						if(UKismetSystemLibrary::DoesImplementInterface(FoundObject->Listeners[CurrentListener], UI_ObjectTagsCommunication::StaticClass()))
						{
							II_ObjectTagsCommunication::Execute_ListeningObjectTagsUpdated(FoundObject->Listeners[CurrentListener], CurrentTag, true, Object, Modifier);
						}
					}
				}

				if(FoundObject->TagRelationships.IsValidIndex(0))
				{
					//Find out if any current relationships would be removed from the application of this tag.
					for(int32 CurrentRelationship = 0; CurrentRelationship < FoundObject->TagRelationships.Num(); CurrentRelationship++)
					{
						UO_TagRelationship* CurrentTagCDO = FoundObject->TagRelationships[CurrentRelationship].GetDefaultObject();
						if(CurrentTagCDO->BlockingTags.HasTagExact(CurrentTag))
						{
							RemoveTagsFromObject_Internal(FGameplayTagContainer({CurrentTagCDO->Tag}), Object, Modifier);
							FoundObject->TagRelationships.RemoveAt(CurrentRelationship);
							CurrentRelationship--;
						}
					}
				}
			
				//For now the success logic is simple, as long as any tag
				//was added, we label this function as a success. - V
				bTagAdded = true;
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

			if(AActor* TargetActor = Cast<AActor>(Object))
			{
				UAbilitySystemBlueprintLibrary::AddLooseGameplayTags(TargetActor, FGameplayTagContainer({CurrentTag}), true);
			}

			//Notify the object itself
			if(UKismetSystemLibrary::DoesImplementInterface(Object, UI_ObjectTagsCommunication::StaticClass()))
			{
				II_ObjectTagsCommunication::Execute_OwningTagsUpdated(Object, CurrentTag, true, Modifier);
			}
		}
		
		ObjectTags.Add(Object, NewObjectTag);

#if WITH_EDITOR
		if(CollectDebuggingData)
		{
			FoundObject = ObjectTags.Find(Object);
		}
#endif
		
		bTagAdded = true;
	}

#if WITH_EDITOR

	if(bTagAdded && CollectDebuggingData)
	{
		if(!FoundObject)
		{
			FoundObject = ObjectTags.Find(Object);
			if(FoundObject)
			{
				FoundObject->TagHistory.Add(FObjectTagHistory(UKismetSystemLibrary::GetDisplayName(Modifier), TagsToAdd, true));
			}
		}
		else
		{
			FoundObject->TagHistory.Add(FObjectTagHistory(UKismetSystemLibrary::GetDisplayName(Modifier), TagsToAdd, true));
		}
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

	FObjectTag* FoundObject = ObjectTags.Find(Object);
	if(FoundObject && FoundObject->Object)
	{
			
		//Check if object already has the tag.
		if(FoundObject->Tags.HasTagExact(TagCDO->Tag))
		{
			return false;
		}

		//Check if object has any of the blocking tags.
		if(FoundObject->Tags.HasAnyExact(TagCDO->BlockingTags))
		{
			return false;
		}

		//Check if the object has all of the required tags.
		if(!FoundObject->Tags.HasAllExact(TagCDO->RequiredTags))
		{
			return false;
		}

		//Remove any tags this relationship wants to remove.
		RemoveTagsFromObject_Internal(TagCDO->RemoveTags, Object, Modifier);

		//Object met requirements, add the tag.
		AddTagsToObject_Internal(FGameplayTagContainer({TagCDO->Tag}), Object, Modifier);

		if(TagCDO->RemoveTagIfAnyBlockingTagIsApplied)
		{
			FoundObject->TagRelationships.Add(TagRelationship);
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
		ObjectTags.Add(Object, NewObjectTag);
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

	FObjectTag* FoundObject = ObjectTags.Find(Object);
	if(FoundObject && FoundObject->Object)
	{
		for(auto& CurrentTag : TagsToRemove)
		{
			if(GetObjectTags(FoundObject->Object).Tags.HasTagExact(CurrentTag))
			{
				FoundObject->Tags.RemoveTag(CurrentTag);

				if(AActor* TargetActor = Cast<AActor>(FoundObject->Object))
				{
					UAbilitySystemBlueprintLibrary::RemoveLooseGameplayTags(TargetActor, FGameplayTagContainer({CurrentTag}), true);
				}

				//Notify the object itself
				if(UKismetSystemLibrary::DoesImplementInterface(Object, UI_ObjectTagsCommunication::StaticClass()))
				{
					II_ObjectTagsCommunication::Execute_OwningTagsUpdated(Object, CurrentTag, false, Modifier);
				}

				//Notify any listeners
				for(auto& CurrentListener : FoundObject->Listeners)
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

	}

#if WITH_EDITOR

	if(bTagRemoved && CollectDebuggingData)
	{
		FoundObject->TagHistory.Add(FObjectTagHistory(UKismetSystemLibrary::GetDisplayName(Modifier), TagsToRemove, false));
	}
	
#endif

	if(FoundObject)
	{
		//If the object has been destroyed or has no tags AND nobody is listening to it, just remove it.
		if(!FoundObject->Object.Get() || (FoundObject->Tags.IsEmpty() && FoundObject->Listeners.IsEmpty()))
		{
			ObjectTags.Remove(Object);
		}
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

	FObjectTag* FoundObject = ObjectTags.Find(OtherObject);
	if(FoundObject && FoundObject->Object)
	{
		if(!FoundObject->Listeners.Contains(Listener))
		{
			FoundObject->Listeners.Add(Listener);
			bListenerAdded = true;
		}
	}
	else
	{
		FObjectTag NewObjectTag;
		NewObjectTag.Object = OtherObject;
		NewObjectTag.Listeners.AddUnique(Listener);
		bListenerAdded = true;
		ObjectTags.Add(OtherObject, NewObjectTag);
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
	FObjectTag* FoundObject = ObjectTags.Find(OtherObject);
	if(FoundObject && FoundObject->Object)
	{
		if(FoundObject->Listeners.Contains(Listener))
		{
			FoundObject->Listeners.Remove(Listener);
			return true;
		}		
	}

	return false;
}

void UObjectTags_Subsystem::CleanupObjectTagArray()
{
	for(auto& CurrentObject : ObjectTags)
	{
		//If an object still has a listener, we do not want to remove the object. Only if an object has no
		//tags and has no listener, then remove it, even though the object is valid.
		if(!CurrentObject.Key.Get() || (CurrentObject.Value.Tags.IsEmpty() && CurrentObject.Value.Listeners.IsEmpty()))
		{
			ObjectTags.Remove(CurrentObject.Key);
		}
	}
}
