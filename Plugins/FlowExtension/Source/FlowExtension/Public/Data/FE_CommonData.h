// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Dialogue/Objects/O_DialogueConditionBase.h"
#include "UObject/Object.h"
#include "FE_CommonData.generated.h"

class UDA_Quest;
class UFlowNode;
class UFlowAsset;
class UWidget;

UENUM(BlueprintType)
enum EQuestState
{
	//The player has not interacted
	//with this quest or task yet.
	Inactive,
	
	InProgress,
	Finished,
	Failed
};

UENUM(BlueprintType)
enum EConditionHandling
{
	AnyCondition,
	AllConditions
};


//--------------//
//	 Dialogue	//

USTRUCT(BlueprintType)
struct FDialogueConditionSettings
{
	GENERATED_BODY()

	/**When we process the conditions, we can control if only a single condition
	 * has to return true or if all of them have to return true.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	TEnumAsByte<EConditionHandling> ConditionHandling = AllConditions;

	/**If the owning dialogue option does NOT want to be hidden, but the requirements
	 * are still not met, the option will be greyed out and not selectable, but still
	 * visible to the player.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option", meta = (EditCondition = "Conditions.IsEmpty() == true", EditConditionHides))
	bool HideIfConditionsAreNotMet = true;

	/**When it's time to present this option to the player, what conditions must
	 * be met?*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Dialogue Option")
	TArray<UO_DialogueConditionBase*> Conditions;
};

USTRUCT(BlueprintType)
struct FDialogueOption
{
	GENERATED_BODY()

	/**The text displayed to the player to choose this option.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FText ButtonText = FText();

	/**The actual dialogue that occurs when the player presses
	 * the dialogue button.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FText DialogueText = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FDialogueConditionSettings ConditionSettings;
};

USTRUCT(BlueprintType)
struct FScript
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FText DialogueText;
};


//----------//


//----------//
//	 Task	//

USTRUCT(BlueprintType)
struct FS_Reward
{
	GENERATED_BODY()

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, float> Rewards;

	UPROPERTY(Category = "Reward", EditAnywhere, BlueprintReadOnly)
	bool bIsAccepted = false;
};

//Requirements for a quest.
//Being left inside a struct in case someone
//needs to add more basic data to this.
USTRUCT(BlueprintType)
struct FTaskRequirement
{
	GENERATED_BODY()

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, float> QuestRequirements;
};

//Fail conditions for a quest.
//Being left inside a struct in case someone
//needs to add more basic data to this.
USTRUCT(BlueprintType)
struct FS_TaskFailCondition
{
	GENERATED_BODY()

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, float> FailConditions;
};

//If you add any data into this struct, remember to go into
//FL_QuestHelpers and update WrapTask.
USTRUCT(BlueprintType)
struct FQuestTask
{
	GENERATED_BODY()

	/**The task itself*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, meta=(Categories="Flow.Quests"))
	FGameplayTag TaskID;
 
    /**Name of the task presented to the player*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	FText TaskName;

	/**How much progress does this task require?*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	float ProgressRequired;

	//Requirements for progressing this task.
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	TArray<FTaskRequirement> Requirements;

	/**What scenarios will fail this task?*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	TArray<FS_TaskFailCondition> FailConditions;

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	TArray<FS_Reward> Rewards;

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	bool IsOptional = false;
};

//If you add any data into this struct, remember to go into
//FL_QuestHelpers and update WrapTask.
USTRUCT(BlueprintType)
struct FTaskWrapper
{
	GENERATED_BODY()
	
	/**The task itself*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, meta=(Categories="Flow.Quests"))
	FGameplayTag TaskID;

    /**Name of the task presented to the player*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	FText TaskName;

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	float CurrentProgress = 0;

	/**How much progress does this task require?*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	float ProgressRequired;

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	TArray<FTaskRequirement> Requirements;

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	TArray<FS_TaskFailCondition> FailConditions;

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	TArray<FS_Reward> Rewards;

	/**What state is the task currently in?*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EQuestState> State = Inactive;

	/**Arbitrary data which can be added to a task, such as a timer, repeat count, etc.*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, meta=(ForceInlineRow), meta=(Categories="Flow.Quests.Metadata"))
	TMap<FGameplayTag, float> Metadata;

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	bool IsOptional = false;

	/**When the task receives interface updates, these objects will receive the same
	 * interface call.
	 * This might be wiped when loading from a save.*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	TArray<UObject*> Listeners;
};

//----------//


//----------//
//	Quest	//


//Requirements for a quest.
//Being left inside a struct in case someone
//needs to add more basic data to this.
USTRUCT(BlueprintType)
struct FQuestRequirement
{
	GENERATED_BODY()

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, float> QuestRequirements;
};

//Fail conditions for a quest.
//Being left inside a struct in case someone
//needs to add more basic data to this.
USTRUCT(BlueprintType)
struct FS_QuestFailCondition
{
	GENERATED_BODY()

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, float> FailConditions;
};

//If you add any data into this struct, remember to go into
//FL_QuestHelpers and update WrapQuest.
USTRUCT(BlueprintType)
struct FQuestWrapper
{
	GENERATED_BODY()

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UDA_Quest> QuestAsset = nullptr;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FTaskWrapper> Tasks;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FQuestRequirement> Requirements;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FS_QuestFailCondition> FailConditions;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FS_Reward> Rewards;

	/**What state is the quest currently in?*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EQuestState> State = Inactive;

	/**Arbitrary data which can be added to a quest, such as a timer, repeat count, etc.*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(ForceInlineRow), meta=(Categories="Flow.Quests.Metadata"))
	TMap<FGameplayTag, float> Metadata;

	/**When the quest receives interface updates, these objects will receive the same
	 * interface call.
	 * This might be wiped when loading from a save.*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<UObject*> Listeners;

	bool operator==(const FQuestWrapper& Argument) const
	{
		if(!QuestAsset.IsValid() || !Argument.QuestAsset.IsValid())
		{
			return false;
		}

		return QuestAsset == Argument.QuestAsset;
	}

	bool operator!=(const FQuestWrapper& Argument) const
	{
		if(!QuestAsset.IsValid() || !Argument.QuestAsset.IsValid())
		{
			return false;
		}

		return QuestAsset != Argument.QuestAsset;
	}
};

//----------//
