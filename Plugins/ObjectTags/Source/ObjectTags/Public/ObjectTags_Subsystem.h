// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "I_ObjectTagsCommunication.h"
#include "O_TagRelationship.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectTags_Subsystem.generated.h"

USTRUCT(BlueprintType)
struct FObjectTagHistory
{
	GENERATED_BODY()

	//Display name of the modifier.
	//Can't use pointer as the actor could be destroyed.
	UPROPERTY(BlueprintReadOnly)
	FString Modifier;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag TagsModified;

	UPROPERTY(BlueprintReadOnly)
	bool Added = false;
};

USTRUCT(BlueprintType)
struct FObjectTag
{
	GENERATED_BODY()

	/**This isn't needed, since this is also the key for the TMap,
	 * but this simplifies a good chunk of logic and takes up tiny
	 * amount of memory.*/
	UPROPERTY(Category = "Object Tags", BlueprintReadOnly)
	TObjectPtr<UObject> Object = nullptr;

	/**What tags have been added with this system?
	 * Keep in mind, you should always use GetObjectTags,
	 * as that will also fetch any external tags.*/
	UPROPERTY(Category = "Object Tags", BlueprintReadOnly)
	TMap<FGameplayTag, float> TagsAndValues;

	UPROPERTY(Category = "Object Tags", BlueprintReadOnly)
	TArray<TObjectPtr<UObject>> Listeners;

	/**Current tag relationships we are tracking. This is only populated
	 * by relationships that are tracking if any blocking tags are applied.*/
	UPROPERTY(Category = "Object Tags", BlueprintReadOnly, SaveGame)
	TArray<TSubclassOf<UO_TagRelationship>> TagRelationships;
	
	/**History of all tags for this object.
	 * Debugging only, do not use for gameplay.*/
	UPROPERTY(Category = "DEVELOPMENT", VisibleAnywhere, BlueprintReadOnly)
	TArray<FObjectTagHistory> TagHistory;
	//TODO: Remove for cooked builds

	/**Timers for current temporary tags.
	 * This map is required to allow for tag durations
	 * to be refreshed.*/
	TMap<FGameplayTag, FTimerHandle> TagTimers;

	bool operator==(const FObjectTag& Argument) const
	{
		return Argument.Object == Object;
	}

	void SetValueForTag(FGameplayTag Tag, float NewValue)
	{
		if(float* FoundTagValue = TagsAndValues.Find(Tag))
		{
			*FoundTagValue = NewValue;
		}
	}

	FGameplayTagContainer GetTagsAsContainer()
	{
		FGameplayTagContainer TagContainer;
		for(auto& CurrentTag : TagsAndValues)
		{
			TagContainer.AddTagFast(CurrentTag.Key);
		}
		return TagContainer;
	}

	void BroadcastTagChange(FGameplayTag Tag, ETagModification Modification, UObject* Modifier, float Value = 1, float Duration = 0)
	{
		//Notify the object itself
		if(UKismetSystemLibrary::DoesImplementInterface(Object, UI_ObjectTagsCommunication::StaticClass()))
		{
			II_ObjectTagsCommunication::Execute_OwningTagsUpdated(Object, Tag, Modification, Modifier, Value, Duration);
		}

		//Notify any listeners
		for(int32 CurrentListener = 0; CurrentListener < Listeners.Num(); CurrentListener++)
		{
			if(!Listeners[CurrentListener])
			{
				continue;
			}

			UObject* Listener = Listeners[CurrentListener];
					
			if(UKismetSystemLibrary::DoesImplementInterface(Listener, UI_ObjectTagsCommunication::StaticClass()))
			{
				II_ObjectTagsCommunication::Execute_ListeningObjectTagsUpdated(Listener, Tag, Modification, Object, Modifier, Value, Duration);
			}

			//In the case that the listener stops listening, the array changes size.
			//Adjust it so we don't skip any objects.
			if(!Listeners.Contains(Listener))
			{
				CurrentListener--;
			}
		}
	}
};

UCLASS()
class OBJECTTAGS_API UObjectTags_Subsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	static UObjectTags_Subsystem* Get();

	UPROPERTY(BlueprintReadWrite)
	bool CollectDebuggingData = true;

	/**V: I don't know why, I couldn't figure out why... But if this is a TSet,
	 * it will VERY randomly crash. It was worrying enough that I don't think it's
	 * worthwhile seeing if it's just an editor problem, especially with how much
	 * work I was losing from the editor crashing.
	 * As a workaround, a TMap will suffice. Even though we are storing the UObject
	 * reference twice now, it's so minor, it won't have any real impact on memory.*/
	UPROPERTY(Category = "Object Tags", BlueprintReadOnly, SaveGame)
	TMap<TObjectPtr<UObject>, FObjectTag> ObjectTags;

	/**Get the tags the system has stored for the object.
	 * TODO: Investigate why this is much more expensive in 5.4*/
	UFUNCTION(Category = "Object Tags", BlueprintCallable, meta=(DefaultToSelf = "Object"))
	static FObjectTag GetObjectTags(UObject* Object);

	/**Adds @TagsToAdd onto the object, triggering listener events along the way.
	 * If the object does not exist in the @ObjectTags array, it'll get added.
	 * @Duration Tags can be applied temporarily, if this value is above 0,
	 * a timer will remove the tag after the duration.
	 * This timer can be reset by calling this function again.*/
	UFUNCTION(Category = "Object Tags", BlueprintCallable, meta=(DefaultToSelf = "Modifier"))
	static bool AddTagToObject(FGameplayTag TagToAdd, UObject* Object, UObject* Modifier, float Value = 1, float Duration = 0);

	/**Adds a tag from the tag relationship onto the object, triggering listener events along the way.
	 * If the object does not exist in the @ObjectTags array, it'll get added.
	 * @Duration Tags can be applied temporarily, if this value is above 0,
	 * a timer will remove the tag after the duration.
	 * This timer can be reset by calling this function again.*/
	UFUNCTION(Category = "Object Tags", BlueprintCallable, meta=(DefaultToSelf = "Modifier"))
	static bool AddTagToObjectWithRelationship(TSubclassOf<UO_TagRelationship> TagRelationship, UObject* Object, UObject* Modifier, float Duration = 0);

	/**Remove a set of tags from an object, triggering listener events along the way.
	 * If the object is not found, this will not do anything.*/
	UFUNCTION(Category = "Object Tags", BlueprintCallable, meta=(DefaultToSelf = "Modifier"))
	static bool RemoveTagsFromObject(FGameplayTagContainer TagsToRemove, UObject* Object, UObject* Modifier);

	UFUNCTION(Category = "Object Tags", BlueprintCallable, BlueprintPure, meta=(DefaultToSelf = "Object"))
	static bool DoesObjectHaveTag(FGameplayTag Tag, UObject* Object);

	/**Gets the value of the @Tag that has been assigned to the @Object*/
	UFUNCTION(Category = "Object Tags", BlueprintCallable, BlueprintPure, meta=(DefaultToSelf = "Object"))
	static float GetTagValueFromObject(FGameplayTag Tag, UObject* Object);

	/**Add a listener for another objects tag updates*/
	UFUNCTION(Category = "Object Tags", BlueprintCallable, meta=(DefaultToSelf = "Listener"))
	static bool AddListenerToObject(UObject* Listener, UObject* OtherObject);

	UFUNCTION(Category = "Object Tags", BlueprintCallable, meta=(DefaultToSelf = "Listener"))
	static bool RemoveListenerForObject(UObject* Listener, UObject* OtherObject);

	/**Called when garbage collector goes off.
	 * This will go through the @ObjectTags array and
	 * remove any indexes that have an invalid object.*/
	void CleanupObjectTagArray();

	/**Get all the tags in the @Container that conflict with the tag
	 * that the @Relationship wants to apply.
	 * As in, tags that would want to be removed if this relationship was applied.
	 * For example; The relationship applies the "Dead" tag. You would clearly
	 * want to remove the "Alive" tag. This would then return that tag, if it
	 * was found in the container.*/
	UFUNCTION(Category = "ObjectTags|Tag Relationship", BlueprintCallable, BlueprintPure)
	static FGameplayTagContainer GetConflictingTags(TSubclassOf<UO_TagRelationship> Relationship, FGameplayTagContainer Container);

	/**Get all the tags in the @Container that are blocking the tag
	 * that the @Relationship wants to apply.*/
	UFUNCTION(Category = "ObjectTags|Tag Relationship", BlueprintCallable, BlueprintPure)
	static FGameplayTagContainer GetBlockingTags(TSubclassOf<UO_TagRelationship> Relationship, FGameplayTagContainer Container);

	/**Does the @Container have the tags that the @Relationship requires to be applied?*/
	UFUNCTION(Category = "ObjectTags|Tag Relationship", BlueprintCallable, BlueprintPure)
	static bool HasRequiredTags(TSubclassOf<UO_TagRelationship> Relationship, FGameplayTagContainer Container);
};
