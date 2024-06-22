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

UObjectTags_Subsystem* UObjectTags_Subsystem::Get()
{
	if(!GEngine->GameViewport) { return nullptr; }
	const UWorld* World = GEngine->GameViewport->GetWorld();
	if(!World) { return nullptr; }
	if(!UGameplayStatics::GetGameInstance(World)){ return nullptr; }
	UObjectTags_Subsystem* SubSystem = UGameplayStatics::GetGameInstance(World)->GetSubsystem<UObjectTags_Subsystem>();
	if(!SubSystem)
	{
		return nullptr;
	}

	return SubSystem;
}

FObjectTag UObjectTags_Subsystem::GetObjectTags(UObject* Object)
{
	if(!Object)
	{
		return FObjectTag();
	}
	
	UObjectTags_Subsystem* ObjectTags_Subsystem = UObjectTags_Subsystem::Get();
	if(!ObjectTags_Subsystem)
	{
		return FObjectTag();
	}

	if(FObjectTag* FoundObject = ObjectTags_Subsystem->ObjectTags.Find(Object))
	{
		return *FoundObject;
	}

	return FObjectTag();
}

bool UObjectTags_Subsystem::AddTagToObject(FGameplayTag TagToAdd, UObject* Object, UObject* Modifier, float Value, float Duration)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AddTagToObject)
	if(!Object)
	{
		return false;
	}
	
	UObjectTags_Subsystem* ObjectTags_Subsystem = UObjectTags_Subsystem::Get();
	if(!ObjectTags_Subsystem)
	{
		return false;
	}

	bool bTagAdded = false;
	FObjectTag* FoundObject = ObjectTags_Subsystem->ObjectTags.Find(Object);
	if(FoundObject)
	{
		//Append the two tag containers and call the TagsModified delegate on the way. - V
		if(!FoundObject->TagsAndValues.Find(TagToAdd))
		{
			FoundObject->TagsAndValues.Add(TagToAdd, Value);

			if(AActor* TargetActor = Cast<AActor>(FoundObject->Object.Get()))
			{
				FGameplayTagContainer ASCTags;
				if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
				{
					AbilitySystemComponent->GetOwnedGameplayTags(ASCTags);
					if(!ASCTags.HasTagExact(TagToAdd))
					{
						UAbilitySystemBlueprintLibrary::AddLooseGameplayTags(TargetActor, FGameplayTagContainer({TagToAdd}), true);
					}
				}
			}

			FoundObject->BroadcastTagChange(TagToAdd, Added, Modifier, Value);

			if(FoundObject->TagRelationships.IsValidIndex(0))
			{
				//Find out if any current relationships would be removed from the application of this tag.
				for(int32 CurrentRelationship = 0; CurrentRelationship < FoundObject->TagRelationships.Num(); CurrentRelationship++)
				{
					UO_TagRelationship* CurrentTagCDO = FoundObject->TagRelationships[CurrentRelationship].GetDefaultObject();
					if(CurrentTagCDO->BlockingTags.HasTagExact(TagToAdd))
					{
						ObjectTags_Subsystem->RemoveTagsFromObject(FGameplayTagContainer({CurrentTagCDO->Tag}), Object, Modifier);
						FoundObject->TagRelationships.RemoveAt(CurrentRelationship);
						CurrentRelationship--;
					}
				}
			}
			
			//For now the success logic is simple, as long as any tag
			//was added, we label this function as a success. - V
			bTagAdded = true;
		}
		else
		{
			//Object was found and already has the tag. Update the value.
			FoundObject->TagsAndValues.Add(TagToAdd, Value);
			bTagAdded = true;
			
			FoundObject->BroadcastTagChange(TagToAdd, Added, Modifier, Value);
		}
	}
	else
	{
		//Object doesn't have the tag, add it
		FObjectTag NewObjectTag;
		NewObjectTag.Object = Object;
		
		NewObjectTag.TagsAndValues.Add(TagToAdd, Value);
		ObjectTags_Subsystem->ObjectTags.Add(Object, NewObjectTag);

		if(AActor* TargetActor = Cast<AActor>(Object))
		{
			FGameplayTagContainer ASCTags;
			if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
			{
				AbilitySystemComponent->GetOwnedGameplayTags(ASCTags);
				if(!ASCTags.HasTagExact(TagToAdd))
				{
					UAbilitySystemBlueprintLibrary::AddLooseGameplayTags(TargetActor, FGameplayTagContainer({TagToAdd}), true);
				}
			}
		}

		NewObjectTag.BroadcastTagChange(TagToAdd, Added, Modifier, Value);

#if WITH_EDITOR
		if(ObjectTags_Subsystem->CollectDebuggingData)
		{
			FoundObject = ObjectTags_Subsystem->ObjectTags.Find(Object);
		}
#endif
		
		bTagAdded = true;
	}

#if WITH_EDITOR

	if(bTagAdded && ObjectTags_Subsystem->CollectDebuggingData)
	{
		if(!FoundObject)
		{
			FoundObject = ObjectTags_Subsystem->ObjectTags.Find(Object);
			if(FoundObject)
			{
				FoundObject->TagHistory.Add(FObjectTagHistory(UKismetSystemLibrary::GetDisplayName(Modifier), TagToAdd, true));
			}
		}
		else
		{
			FoundObject->TagHistory.Add(FObjectTagHistory(UKismetSystemLibrary::GetDisplayName(Modifier), TagToAdd, true));
		}
	}
	
#endif

	//Set up the timer to have the tag automatically remove itself
	if(bTagAdded && Duration > 0)
	{
		//Find and clear any current timers for this tag, so we can refresh the duration.
		if(FTimerHandle* OldHandle =FoundObject->TagTimers.Find(TagToAdd))
		{
			ObjectTags_Subsystem->GetWorld()->GetTimerManager().ClearTimer(*OldHandle);
		}

		//Start the timer and pass the needed variables to remove the tag in the future.
		FTimerHandle NewHandle;
		ObjectTags_Subsystem->GetWorld()->GetTimerManager().SetTimer(NewHandle, FTimerDelegate::CreateLambda([TagToAdd, Object, Modifier]
		{
			if(Object)
			{
				UObjectTags_Subsystem::RemoveTagsFromObject(FGameplayTagContainer({TagToAdd}), Object, Modifier ? Modifier : nullptr);
			}
		}), Duration, false);

		//Add the handle so it's possible to refresh the duration if we try to add the tag again.
		FoundObject->TagTimers.Add(TagToAdd, NewHandle);
	}

	return bTagAdded;
}

bool UObjectTags_Subsystem::AddTagToObjectWithRelationship(TSubclassOf<UO_TagRelationship> TagRelationship, UObject* Object,
                                                           UObject* Modifier, float Duration)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(AddTagToObjectWithRelationship)
	UObjectTags_Subsystem* ObjectTags_Subsystem = UObjectTags_Subsystem::Get();
	if(!ObjectTags_Subsystem)
	{
		return false;
	}
	
	UO_TagRelationship* TagCDO = TagRelationship.GetDefaultObject();
	if(!TagCDO)
	{
		return false;	
	}

	FObjectTag* FoundObject = ObjectTags_Subsystem->ObjectTags.Find(Object);
	if(FoundObject && FoundObject->Object)
	{
		//Logic is a lot simpler by just using the TagContainer helper functions
		FGameplayTagContainer TagContainer = FoundObject->GetTagsAsContainer();
			
		//Check if object already has the tag.
		if(TagContainer.HasTagExact(TagCDO->Tag))
		{
			return false;
		}

		//Check if object has any of the blocking tags.
		if(TagContainer.HasAnyExact(TagCDO->BlockingTags))
		{
			return false;
		}

		//Check if the object has all the required tags.
		if(!HasRequiredTags(TagRelationship, TagContainer))
		{
			return false;
		}

		//Remove any tags this relationship wants to remove.
		RemoveTagsFromObject(TagCDO->RemoveTags, Object, Modifier);

		//Object met requirements, add the tag.
		AddTagToObject(TagCDO->Tag, Object, Modifier, TagCDO->Value, Duration);

		if(TagCDO->RemoveTagIfAnyBlockingTagIsApplied)
		{
			FoundObject->TagRelationships.Add(TagRelationship);
		}

		return true;
	}
	else
	{
		FObjectTag NewObjectTag;
		NewObjectTag.Object = Object;
		NewObjectTag.TagsAndValues.Add(TagCDO->Tag, TagCDO->Value);

		FoundObject->BroadcastTagChange(TagCDO->Tag, Added, Modifier, TagCDO->Value);
		
		ObjectTags_Subsystem->ObjectTags.Add(Object, NewObjectTag);
		return true;
	}
}

bool UObjectTags_Subsystem::RemoveTagsFromObject(FGameplayTagContainer TagsToRemove, UObject* Object, UObject* Modifier)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(RemoveTagsFromObject)
	UObjectTags_Subsystem* ObjectTags_Subsystem = UObjectTags_Subsystem::Get();
	if(!ObjectTags_Subsystem)
	{
		return false;
	}

	bool bTagRemoved = false;

	FObjectTag* FoundObject = ObjectTags_Subsystem->ObjectTags.Find(Object);
	if(FoundObject && FoundObject->Object)
	{
		for(auto& CurrentTag : TagsToRemove)
		{
			if(FoundObject->TagsAndValues.Remove(CurrentTag))
			{
				if(AActor* TargetActor = Cast<AActor>(FoundObject->Object))
				{
					UAbilitySystemBlueprintLibrary::RemoveLooseGameplayTags(TargetActor, FGameplayTagContainer({CurrentTag}), true);
				}

				FoundObject->BroadcastTagChange(CurrentTag, Removed, Modifier, 0);

				//For now the success logic is simple, as long as any tag
				//was added, we label this function as a success. - V
				bTagRemoved = true;
			}
			
			#if WITH_EDITOR
			if(bTagRemoved && ObjectTags_Subsystem->CollectDebuggingData)
			{
				FoundObject->TagHistory.Add(FObjectTagHistory(UKismetSystemLibrary::GetDisplayName(Modifier), CurrentTag, false));
			}
			#endif
		}
	}

	if(FoundObject)
	{
		//If the object has been destroyed or has no tags AND nobody is listening to it, just remove it.
		if(!FoundObject->Object.Get() || (FoundObject->TagsAndValues.IsEmpty() && FoundObject->Listeners.IsEmpty()))
		{
			ObjectTags_Subsystem->ObjectTags.Remove(Object);
		}
	}
	
	return bTagRemoved;
}

bool UObjectTags_Subsystem::DoesObjectHaveTag(FGameplayTag Tag, UObject* Object)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(DoesObjectHaveTag)
	UObjectTags_Subsystem* ObjectTags_Subsystem = UObjectTags_Subsystem::Get();
	if(!ObjectTags_Subsystem)
	{
		return false;
	}
	
	if(FObjectTag* ObjectTags = ObjectTags_Subsystem->ObjectTags.Find(Object))
	{
		if(ObjectTags->TagsAndValues.Contains(Tag))
		{
			return true;
		}
	}

	return false;
}

float UObjectTags_Subsystem::GetTagValueFromObject(FGameplayTag Tag, UObject* Object)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(GetTagValueFromObject)
	UObjectTags_Subsystem* ObjectTags_Subsystem = UObjectTags_Subsystem::Get();
	if(!ObjectTags_Subsystem)
	{
		return 0;
	}

	if(FObjectTag* ObjectTags = ObjectTags_Subsystem->ObjectTags.Find(Object))
	{
		if(float* Value = ObjectTags->TagsAndValues.Find(Tag))
		{
			return *Value;
		}
	}

	return 0;
}

bool UObjectTags_Subsystem::AddListenerToObject(UObject* Listener, UObject* OtherObject)
{
	UObjectTags_Subsystem* ObjectTags_Subsystem = UObjectTags_Subsystem::Get();
	if(!ObjectTags_Subsystem)
	{
		return false;
	}

	bool bListenerAdded = false;

	FObjectTag* FoundObject = ObjectTags_Subsystem->ObjectTags.Find(OtherObject);
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
		ObjectTags_Subsystem->ObjectTags.Add(OtherObject, NewObjectTag);
	}

	return bListenerAdded;
}

bool UObjectTags_Subsystem::RemoveListenerForObject(UObject* Listener, UObject* OtherObject)
{
	UObjectTags_Subsystem* ObjectTags_Subsystem = UObjectTags_Subsystem::Get();
	if(!ObjectTags_Subsystem)
	{
		return false;
	}

	FObjectTag* FoundObject = ObjectTags_Subsystem->ObjectTags.Find(OtherObject);
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
	TRACE_CPUPROFILER_EVENT_SCOPE(CleanupObjectTagArray)
	TArray<TObjectPtr<UObject>> ObjectArray;
	ObjectTags.GenerateKeyArray(ObjectArray);

	TArray<FObjectTag> TagsArray;
	ObjectTags.GenerateValueArray(TagsArray);
	
	for(int32 CurrentObject = 0; CurrentObject < ObjectArray.Num(); CurrentObject++)
	{
		//If an object still has a listener, we do not want to remove the object. Only if an object has no
		//tags and has no listener, then remove it, even though the object is valid.
		if(!ObjectArray[CurrentObject].Get() || (TagsArray[CurrentObject].TagsAndValues.IsEmpty() && TagsArray[CurrentObject].Listeners.IsEmpty()))
		{
			ObjectTags.Remove(ObjectArray[CurrentObject]);
			ObjectArray.RemoveAt(CurrentObject);
			TagsArray.RemoveAt(CurrentObject);
			CurrentObject--;
		}
	}
}

FGameplayTagContainer UObjectTags_Subsystem::GetConflictingTags(TSubclassOf<UO_TagRelationship> Relationship, FGameplayTagContainer Container)
{
	FGameplayTagContainer TagsToRemove;

	for(auto& CurrentTag : Container)
	{
		if(Relationship.GetDefaultObject()->RemoveTags.HasTagExact(CurrentTag))
		{
			TagsToRemove.AddTagFast(CurrentTag);
		}
	}

	return TagsToRemove;
}

FGameplayTagContainer UObjectTags_Subsystem::GetBlockingTags(TSubclassOf<UO_TagRelationship> Relationship,
	FGameplayTagContainer Container)
{
	FGameplayTagContainer BlockingTags;

	for(auto& CurrentTag : Container)
	{
		if(Relationship.GetDefaultObject()->BlockingTags.HasTagExact(CurrentTag))
		{
			BlockingTags.AddTagFast(CurrentTag);
		}
	}

	return BlockingTags;
}

bool UObjectTags_Subsystem::HasRequiredTags(TSubclassOf<UO_TagRelationship> Relationship,
	FGameplayTagContainer Container)
{
	return Container.HasAllExact(Relationship.GetDefaultObject()->RequiredTags);
}
