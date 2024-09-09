// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "AC_ObjectTags.generated.h"

USTRUCT(BlueprintType)
struct FSimpleObjectTag
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag Tag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Value = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Duration = 0;
};

/**Basic component that does nothing but add tags to the owner of this
 * component on BeginPlay, then it will destroy itself.
 * This is primarily used for level designers who want to add a tag
 * to a unique actor without modifying its code.*/
UCLASS(ClassGroup=(ObjectTags), meta=(BlueprintSpawnableComponent))
class OBJECTTAGS_API UAC_ObjectTags : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "Tag"))
	TArray<FSimpleObjectTag> TagsToGrant;
	
	UAC_ObjectTags();

protected:
	
	virtual void BeginPlay() override;
};
