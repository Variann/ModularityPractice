// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "I_ObjectTagsCommunication.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UI_ObjectTagsCommunication : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OBJECTTAGS_API II_ObjectTagsCommunication
{
	GENERATED_BODY()

public:

	//Most actors should have tags, this can be used to gain context for interaction/communication.
	UFUNCTION(Category = "Communication||Actor||Tags", BlueprintNativeEvent, BlueprintCallable, meta = (DeprecatedFunction))
	FGameplayTagContainer GetObjectTags();

	/**Get any tags the targeted object has, which is not participating in
	 * the object tags system.
	 * These tags can not be removed, the object tags subsystem will not
	 * manage these tags. This is where you could implement permanent tags.*/
	UFUNCTION(Category = "Communication||Actor||Tags", BlueprintNativeEvent, BlueprintCallable)
	FGameplayTagContainer GetObjectsExternalTags();

	/*Attempt to append this tag container to this actors tag container.
	 If the receiver is a child of CoreActor, CoreGameplayActor, CorePawn,
	 or ParentWidget they will automatically call their TagsModified delegate.*/
	UFUNCTION(Category = "Communication||Actor||Tags", BlueprintNativeEvent, BlueprintCallable, meta = (DeprecatedFunction))
	bool AddTagsToObject(FGameplayTagContainer TagsToAdd, UObject* Modifier);

	/*Attempt to remove all specified tags from this actors tag container.
	If the receiver is a child of CoreActor, CoreGameplayActor or CorePawn,
	they will automatically call their TagsModified delegate.
	This only removes exact matches.*/
	UFUNCTION(Category = "Communication||Actor||Tags", BlueprintNativeEvent, BlueprintCallable, meta = (DeprecatedFunction))
	bool RemoveTagsFromObject(FGameplayTagContainer TagsToRemove, UObject* Modifier);

	/**The one who receives this function call has had its tags updated
	 * in some way.*/
	UFUNCTION(Category = "ObjectTags", BlueprintNativeEvent, BlueprintCallable)
	void OwningTagsUpdated(FGameplayTag Tag, bool Added, UObject* Modifier);

	/**The one who receives this function call is listening to another objects
	 * tag updates, and @OwningTagsUpdated has been called on that object.*/
	UFUNCTION(Category = "ObjectTags", BlueprintNativeEvent, BlueprintCallable)
	void ListeningObjectTagsUpdated(FGameplayTag Tag, bool Added, UObject* Object, UObject* Modifier);
};
