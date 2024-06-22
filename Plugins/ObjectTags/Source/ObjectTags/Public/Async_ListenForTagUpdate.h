// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "I_ObjectTagsCommunication.h"
#include "Engine/CancellableAsyncAction.h"
#include "Async_ListenForTagUpdate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FListenForTagUpdate, FGameplayTag, Tag, ETagModification, Modification, UObject*, Object,
	UObject*, Modifier, float, NewValue, float, Duration);

/**
 * 
 */
UCLASS()
class OBJECTTAGS_API UAsync_ListenForTagUpdate : public UCancellableAsyncAction, public II_ObjectTagsCommunication
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "ObjectTags", meta = (BlueprintInternalUseOnly = "true"))
	static UAsync_ListenForTagUpdate* ListenForTagUpdate(UObject* Object, FGameplayTag Tag);

	virtual void Activate() override;
	virtual void Cancel() override;

	UPROPERTY()
	UObject* ObjectToTrack = nullptr;
	FGameplayTag TagToTrack;

	UPROPERTY(BlueprintAssignable)
	FListenForTagUpdate OnTagUpdate;

	virtual void ListeningObjectTagsUpdated_Implementation(FGameplayTag Tag, ETagModification Modification, UObject* Object, UObject* Modifier, float NewValue, float Duration) override;
};
