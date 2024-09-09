// Copyright (C) Varian Daemon. All Rights Reserved


#include "ObjectTags_Subsystem.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "I_ObjectTagsCommunication.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(ObjectTagsLog, Log, All)

void UObjectTags_Subsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ObjectTags.Empty();

	FCoreUObjectDelegates::GetPreGarbageCollectDelegate().AddUObject(this, &UObjectTags_Subsystem::CleanupObjectTagArray);
	
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("AddTag"),
		TEXT("Adds a tag to the actor in front of the camera."),
			FConsoleCommandWithArgsDelegate::CreateStatic(&AddTagConsoleCommand),
			ECVF_Default);

	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("RemoveTag"),
		TEXT("Remove a tag from the actor in front of the camera."),
			FConsoleCommandWithArgsDelegate::CreateStatic(&RemoveTagConsoleCommand),
			ECVF_Default);

	IConsoleManager::Get().RegisterConsoleCommand(
	TEXT("AddTagToPlayer"),
	TEXT("Adds a tag to the player pawn"),
		FConsoleCommandWithArgsDelegate::CreateStatic(&AddTagToPlayerConsoleCommand),
		ECVF_Default);

	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("RemoveTagFromPlayer"),
		TEXT("Remove a tag from the player pawn"),
			FConsoleCommandWithArgsDelegate::CreateStatic(&RemoveTagFromPlayerConsoleCommand),
			ECVF_Default);
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
			AActor* TargetActor = Cast<AActor>(FoundObject->Object.Get());
			if(TargetActor)
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

			#if ENABLE_VISUAL_LOG
			if(TargetActor)
			{
				UE_VLOG_LOCATION(ObjectTags_Subsystem, ObjectTagsLog, Verbose, TargetActor->GetActorLocation(),
				3, FColor::White, TEXT("Added tag %s to %s, added by %s"), *TagToAdd.ToString(), *Object->GetName(), *Modifier->GetName());
			}
			else
			{
				UE_VLOG(ObjectTags_Subsystem, ObjectTagsLog, Verbose, TEXT("Added tag tag %s to %s, added by %s"), *TagToAdd.ToString(), *Object->GetName(), *Modifier->GetName());
			}

			UE_VLOG_UELOG(ObjectTags_Subsystem, ObjectTagsLog, Verbose, TEXT("Added tag tag %s to %s, added by %s"), *TagToAdd.ToString(), *Object->GetName(), *Modifier->GetName());
			#endif

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

			#if ENABLE_VISUAL_LOG
			AActor* TargetActor = Cast<AActor>(FoundObject->Object.Get());
			if(TargetActor)
			{
				UE_VLOG_LOCATION(ObjectTags_Subsystem, ObjectTagsLog, Verbose, TargetActor->GetActorLocation(),
				3, FColor::White, TEXT("Tag value %s set to %f on %s, modified by %s"), *TagToAdd.ToString(), Value, *Object->GetName(), *Modifier->GetName());
			}
			else
			{
				UE_VLOG(ObjectTags_Subsystem, ObjectTagsLog, Verbose, TEXT("Tag value %s set to %f on %s, modified by %s"), *TagToAdd.ToString(), Value, *Object->GetName(), *Modifier->GetName());
			}
			UE_VLOG_UELOG(ObjectTags_Subsystem, ObjectTagsLog, Verbose, TEXT("Tag value %s set to %f on %s, modified by %s"), *TagToAdd.ToString(), Value, *Object->GetName(), *Modifier->GetName());
			#endif
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
				UObjectTags_Subsystem::RemoveTagsFromObject(FGameplayTagContainer({TagToAdd}), Object, IsValid(Modifier) ? Modifier : nullptr);
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
				AActor* TargetActor = Cast<AActor>(FoundObject->Object);
				if(TargetActor)
				{
					UAbilitySystemBlueprintLibrary::RemoveLooseGameplayTags(TargetActor, FGameplayTagContainer({CurrentTag}), true);
				}

				FoundObject->BroadcastTagChange(CurrentTag, Removed, Modifier, 0);

				#if ENABLE_VISUAL_LOG

				if(TargetActor)
				{
					if(Modifier)
					{
						UE_VLOG_LOCATION(ObjectTags_Subsystem, ObjectTagsLog, Verbose, TargetActor->GetActorLocation(),
						3, FColor::Yellow, TEXT("Removed tag %s from %s, removed by %s"), *CurrentTag.ToString(), *Object->GetName(), *Modifier->GetName());
					}
					else
					{
						UE_VLOG_LOCATION(ObjectTags_Subsystem, ObjectTagsLog, Verbose, TargetActor->GetActorLocation(),
						3, FColor::Yellow, TEXT("Removed tag %s from %s, removed by invalid object"), *CurrentTag.ToString(), *Object->GetName());
					}
				}
				else
				{
					if(Modifier)
					{
						UE_VLOG(ObjectTags_Subsystem, ObjectTagsLog, Verbose, TEXT("Removed tag %s from %s, removed by %s"), *CurrentTag.ToString(), *Object->GetName(), *Modifier->GetName());
					}
					else
					{
						UE_VLOG(ObjectTags_Subsystem, ObjectTagsLog, Verbose, TEXT("Removed tag %s from %s, removed by invalid object"), *CurrentTag.ToString(), *Object->GetName());
					}
				}

				if(Modifier)
				{
					UE_VLOG_UELOG(ObjectTags_Subsystem, ObjectTagsLog, Verbose, TEXT("Removed tag %s from %s, removed by %s"), *CurrentTag.ToString(), *Object->GetName(), *Modifier->GetName());
				}
				else
				{
					UE_VLOG_UELOG(ObjectTags_Subsystem, ObjectTagsLog, Verbose, TEXT("Removed tag %s from %s, removed by invalid object"), *CurrentTag.ToString(), *Object->GetName());
				}

				#endif

				//For now the success logic is simple, as long as any tag
				//was added, we label this function as a success. - V
				bTagRemoved = true;
			}
			
			#if WITH_EDITOR
			if(bTagRemoved && ObjectTags_Subsystem->CollectDebuggingData)
			{
				FoundObject->TagHistory.Add(FObjectTagHistory( Modifier ? UKismetSystemLibrary::GetDisplayName(Modifier) : "Invalid Object", CurrentTag, false));
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

AActor* UObjectTags_Subsystem::GetActorForConsoleCommand()
{
	UObjectTags_Subsystem* Tags_Subsystem = UObjectTags_Subsystem::Get();
	if(!Tags_Subsystem)
	{
		return nullptr;
	}
	
	FVector Location = UGameplayStatics::GetPlayerCameraManager(Tags_Subsystem, 0)->GetCameraLocation();
	FRotator Rotation = UGameplayStatics::GetPlayerCameraManager(Tags_Subsystem, 0)->GetCameraRotation();
	FVector DirectionVector = Rotation.Vector();
	FVector EndLocation = (DirectionVector * 500) + Location;
	FHitResult HitTarget;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(Tags_Subsystem, 0));
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	Tags_Subsystem->GetWorld()->LineTraceSingleByObjectType(HitTarget, Location, EndLocation, QueryParams, CollisionParams);
	DrawDebugSphere(Tags_Subsystem->GetWorld(), HitTarget.bBlockingHit ? HitTarget.Location : EndLocation, 10, 12, HitTarget.bBlockingHit ? FColor::Green : FColor::Red, false, 5);
	return HitTarget.GetActor();
}

void UObjectTags_Subsystem::AddTagConsoleCommand(const TArray<FString>& Args)
{
	AddTagConsoleCommandInternal(Args, false);
}

void UObjectTags_Subsystem::AddTagToPlayerConsoleCommand(const TArray<FString>& Args)
{
	AddTagConsoleCommandInternal(Args, true);
}

void UObjectTags_Subsystem::AddTagConsoleCommandInternal(const TArray<FString>& Args, bool AddToPlayer)
{
	UObjectTags_Subsystem* Tags_Subsystem = UObjectTags_Subsystem::Get();
	if(!Tags_Subsystem)
	{
		return;
	}

	if(Args.Num() < 1)
	{
		return;
	}

	FGameplayTag TagToAdd = UGameplayTagsManager::Get().RequestGameplayTag(FName(*Args[0]));
	if(!TagToAdd.IsValid())
	{
		UKismetSystemLibrary::PrintString(Tags_Subsystem, "Tag not found");
	}

	if(Args.IsValidIndex(1))
	{
		if(!Args[1].IsNumeric())
		{
			UKismetSystemLibrary::PrintString(Tags_Subsystem, "Value parameter was not numeric");
			return;
		}
	}

	if(Args.IsValidIndex(2))
	{
		if(!Args[2].IsNumeric())
		{
			UKismetSystemLibrary::PrintString(Tags_Subsystem, "Duration parameter was not numeric");
			return;
		}
	}

	float Value = Args.IsValidIndex(1) ? FCString::Atof(*Args[1]) : 1;
	float Duration = Args.IsValidIndex(2) ? FCString::Atof(*Args[2]) : 0;

	if(AddToPlayer)
	{
		UObjectTags_Subsystem::AddTagToObject(TagToAdd, UGameplayStatics::GetPlayerPawn(Tags_Subsystem, 0), Tags_Subsystem, Value, Duration);
	}
	else
	{
		if(AActor* Target = GetActorForConsoleCommand())
		{
			UObjectTags_Subsystem::AddTagToObject(TagToAdd, Target, Tags_Subsystem, Value, Duration);
		}
	}
}

void UObjectTags_Subsystem::RemoveTagConsoleCommand(const TArray<FString>& Args)
{
	RemoveTagFConsoleCommandInternal(Args, false);
}

void UObjectTags_Subsystem::RemoveTagFromPlayerConsoleCommand(const TArray<FString>& Args)
{
	RemoveTagFConsoleCommandInternal(Args, true);
}

void UObjectTags_Subsystem::RemoveTagFConsoleCommandInternal(const TArray<FString>& Args, bool RemoveFromPlayer)
{
	UObjectTags_Subsystem* Tags_Subsystem = UObjectTags_Subsystem::Get();
	if(!Tags_Subsystem)
	{
		return;
	}

	if(Args.Num() != 1)
	{
		return;
	}

	FGameplayTag TagToRemove = UGameplayTagsManager::Get().RequestGameplayTag(FName(*Args[0]));
	if(!TagToRemove.IsValid())
	{
		UKismetSystemLibrary::PrintString(Tags_Subsystem, "Tag not found");
	}

	if(RemoveFromPlayer)
	{
		UObjectTags_Subsystem::RemoveTagsFromObject(FGameplayTagContainer(TagToRemove), UGameplayStatics::GetPlayerPawn(Tags_Subsystem, 0), Tags_Subsystem);
	}
	else
	{
		if(AActor* Target = GetActorForConsoleCommand())
		{
			UObjectTags_Subsystem::RemoveTagsFromObject(FGameplayTagContainer(TagToRemove), Target, Tags_Subsystem);
		}
	}
}
