// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TagsData.generated.h"

//The object is the class who's tags were modified.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FTagsModified, FGameplayTag, Tag, bool, Added, UObject*, Object, UObject*, Modifier);


//TODO: Find out why the delegate becomes undiscoverable to other classes without the UCLASS.
//This is not a problem in other projects, only in this plugin.
UCLASS()
class OBJECTTAGS_API UTagsData : public UObject
{
	GENERATED_BODY()
};
