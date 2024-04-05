// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "O_TagRelationship.h"
#include "Subsystems/LocalPlayerSubsystem.h"
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
	FGameplayTagContainer  TagsModified;

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
	FGameplayTagContainer Tags;

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

	bool operator==(const FObjectTag& Argument) const
	{
		return Argument.Object == Object;
	}
};

UCLASS()
class OBJECTTAGS_API UObjectTags_Subsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(BlueprintReadWrite)
	bool CollectDebuggingData = true;

	UPROPERTY(Category = "Object Tags", BlueprintReadOnly, SaveGame)
	TMap<TObjectPtr<UObject>, FObjectTag> ObjectTags;
	/**V: I don't know why, I couldn't figure out why... But if this is a TSet,
	 * it will VERY randomly crash. It was worrying enough that I don't think it's
	 * worthwhile seeing if it's just an editor problem, especially with how much
	 * work I was losing from the editor crashing.
	 * If finding the array index becomes too expensive, then start considering
	 * reworking this to a TSet. But until then, this will stay as a TArray.*/

	//Most actors should have tags, this can be used to gain context for interaction/communication.
	UFUNCTION(Category = "Object Tags", BlueprintCallable, meta=(DefaultToSelf = "Object"))
	static FObjectTag GetObjectTags(UObject* Object);
	FObjectTag GetObjectTags_Internal(UObject* Object);

	/**Adds @TagsToAdd onto the object, triggering listener events along the way.
	 * If the object does not exist in the @ObjectTags array, it'll get added.*/
	UFUNCTION(Category = "Object Tags", BlueprintCallable, meta=(DefaultToSelf = "Modifier"))
	static bool AddTagsToObject(FGameplayTagContainer TagsToAdd, UObject* Object, UObject* Modifier);
	bool AddTagsToObject_Internal(FGameplayTagContainer TagsToAdd, UObject* Object, UObject* Modifier);

	/**Adds a tag from the tag relationship onto the object, triggering listener events along the way.
	 * If the object does not exist in the @ObjectTags array, it'll get added.*/
	UFUNCTION(Category = "Object Tags", BlueprintCallable, meta=(DefaultToSelf = "Modifier"))
	static bool AddTagToObjectWithRelationship(TSubclassOf<UO_TagRelationship> TagRelationship, UObject* Object, UObject* Modifier);
	bool AddTagToObjectWithRelationship_Internal(TSubclassOf<UO_TagRelationship> TagRelationship, UObject* Object, UObject* Modifier);

	/**Remove a set of tags from an object, triggering listener events along the way.
	 * If the object is not found, this will not do anything.*/
	UFUNCTION(Category = "Object Tags", BlueprintCallable, meta=(DefaultToSelf = "Modifier"))
	static bool RemoveTagsFromObject(FGameplayTagContainer TagsToRemove, UObject* Object, UObject* Modifier);
	bool RemoveTagsFromObject_Internal(FGameplayTagContainer TagsToRemove, UObject* Object, UObject* Modifier);

	UFUNCTION(Category = "Object Tags", BlueprintCallable, BlueprintPure, meta=(DefaultToSelf = "Object"))
	static bool DoesObjectHaveTag(FGameplayTag Tag, UObject* Object);

	/**Add a listener for another objects tag updates*/
	UFUNCTION(Category = "Object Tags", BlueprintCallable, meta=(DefaultToSelf = "Listener"))
	static bool AddListenerToObject(UObject* Listener, UObject* OtherObject);
	bool AddListenerToObject_Internal(UObject* Listener, UObject* OtherObject);

	UFUNCTION(Category = "Object Tags", BlueprintCallable, meta=(DefaultToSelf = "Listener"))
	static bool RemoveListenerForObject(UObject* Listener, UObject* OtherObject);
	bool RemoveListenerForObject_Internal(UObject* Listener, UObject* OtherObject);

	/**Called when garbage collector goes off.
	 * This will go through the @ObjectTags array and
	 * remove any indexes that have an invalid object.*/
	void CleanupObjectTagArray();
};
