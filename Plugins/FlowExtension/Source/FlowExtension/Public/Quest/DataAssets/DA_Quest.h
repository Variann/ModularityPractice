// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DA_Quest.generated.h"

#if WITH_EDITOR
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDataAssetChanged);
#endif

class UDA_QuestChain;
struct FS_Reward;
struct FS_QuestFailCondition;
struct FQuestRequirement;
struct FQuestTask;
class UO_TaskRequirementBase;
class UO_TaskFailConditionBase;

/**Base class for all quests.
 * This class should have very little data and be
 * extremely fast to load.
 * This class is instantly loaded in a few places,
 * so it is important that it is small enough to not
 * cause any hitches.*/
UCLASS(meta=(ShowWorldContextPin, ContextMenuCategory = "Varian's Plugins", ContextMenuEntryName = "Quest|Quest Asset", ContextMenuPrefix = "QA_"))
class FLOWEXTENSION_API UDA_Quest : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(Categories="Quests"))
	FGameplayTag QuestID;

	/**Name of the quest presented to the player*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	FText QuestName;
	
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta = (MultiLine))
	FText QuestText;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta = (TitleProperty = "TaskID"))
	TArray<FQuestTask> Tasks;

	/**Requirements to accept the quest.*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FQuestRequirement> Requirements;

	/**What scenarios will fail this quest?*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FS_QuestFailCondition> FailConditions;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<TSoftObjectPtr<UDA_QuestChain>> QuestChains;

	/**What tags are associated with this quest?
	 * This has no default implementation, you will need
	 * to write any implementation yourself.*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	FGameplayTag Tags;

	/**Arbitrary data which can be added to a quest, such as a timer, repeat count, etc.*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, float> TagValues;

	UFUNCTION(Category = "Quest", BlueprintCallable)
	bool IsTaskOptional(FGameplayTag TaskID);

	UFUNCTION(Category = "Quest", BlueprintCallable)
	float GetRequiredTaskProgression(FGameplayTag TaskID);

	UFUNCTION(Category = "Quest", BlueprintCallable)
	TArray<UO_TaskRequirementBase*> GetTasksRequirements(FGameplayTag TaskID);

	//Editor
	
#if WITH_EDITORONLY_DATA

	//If EditorAssistant is installed, this will override the thumbnail automatically.
	UPROPERTY(meta = (ThumbnailOverride))
	UTexture2D* ThumbnailOverride = Cast<UTexture2D>(StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("/FlowExtension/Quest/T_QuestIcon_Editor.T_QuestIcon_Editor"))) ;

	FOnDataAssetChanged OnDataAssetChanged;
	
#endif

#if WITH_EDITOR

	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
};
