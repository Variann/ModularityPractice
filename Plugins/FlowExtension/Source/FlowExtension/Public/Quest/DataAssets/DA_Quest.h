// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/FE_CommonData.h"
#include "Engine/DataAsset.h"
#include "DA_Quest.generated.h"

/**Base class for all quests.
 * This class should have very little data and be
 * extremely fast to load.
 * This class is instantly loaded in a few places,
 * so it is important that it is small enough to not
 * cause any hitches.*/

UCLASS()
class FLOWEXTENSION_API UDA_Quest : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	FGameplayTag QuestID;

	/**Name of the quest presented to the player*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	FText QuestName;
	
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	FText QuestText;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FQuestTask> Tasks;

	/**Requirements to accept the quest.*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FQuestRequirement> Requirements;

	/**What scenarios will fail this quest?*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FS_QuestFailCondition> FailConditions;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FS_Reward> Rewards;

	/**Arbitrary data which can be added to a quest, such as a timer, repeat count, etc.*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(ForceInlineRow), meta=(Categories="Flow.Quests.Metadata"))
	TMap<FGameplayTag, float> Metadata;
};
