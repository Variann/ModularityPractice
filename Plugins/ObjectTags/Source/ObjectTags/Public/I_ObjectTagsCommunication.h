// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "I_ObjectTagsCommunication.generated.h"

UENUM(BlueprintType)
enum ETagModification
{
	Added,
	Removed,
	ValueModified
};

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

	/**Get any tags the targeted object has, which is not participating in
	 * the object tags system.
	 * These are tags the object tags subsystem will not manage.
	 * This is where you could implement permanent tags.*/
	UFUNCTION(Category = "ObjectTags", BlueprintNativeEvent, BlueprintCallable)
	FGameplayTagContainer GetObjectsExternalTags();

	UFUNCTION(Category = "ObjectTags", BlueprintNativeEvent, BlueprintCallable)
	TMap<FGameplayTag, float> GetObjectsExternalTagsAndValues();

	/**The one who receives this function call has had its tags updated
	 * in some way.*/
	UFUNCTION(Category = "ObjectTags", BlueprintNativeEvent, BlueprintCallable)
	void OwningTagsUpdated(FGameplayTag Tag, ETagModification Modification, UObject* Modifier, float NewValue, float Duration);

	/**The one who receives this function call is listening to another objects
	 * tag updates, and @OwningTagsUpdated has been called on that object.*/
	UFUNCTION(Category = "ObjectTags", BlueprintNativeEvent, BlueprintCallable)
	void ListeningObjectTagsUpdated(FGameplayTag Tag, ETagModification Modification, UObject* Object, UObject* Modifier, float NewValue, float Duration);
};
