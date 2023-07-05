// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Dialogue/Objects/O_DialogueConditionBase.h"
#include "UObject/Object.h"
#include "FE_CommonData.generated.h"

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
	SingularCondition,
	AllConditions
};

USTRUCT(BlueprintType)
struct FS_FlowPin
{
	GENERATED_BODY()

	// A logical name, used during execution of pin
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowPin")
	FName PinName;

	// An optional Display Name, you can use it to override PinName without the need to update graph connections
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowPin")
	FText PinFriendlyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowPin")
	FString PinToolTip;
};


//--------------//
//	 Dialogue	//

USTRUCT(BlueprintType)
struct FS_DialogueSettings
{
	GENERATED_BODY()

	/**The text displayed to the player to choose this option.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FText ButtonText = FText();

	/**The actual dialogue that occurs when the player presses
	 * the dialogue button.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FText DialogueText = FText();
};

USTRUCT(BlueprintType)
struct FS_DialogueOption
{
	GENERATED_BODY()
	
	/**The actual dialogue that occurs when the player presses
	 * the dialogue button.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FS_DialogueSettings DialogueSettings;

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
struct FS_Script
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
struct FS_TaskRequirement
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
struct FS_QuestTask
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
	TArray<FS_TaskRequirement> Requirements;

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
struct FS_TaskWrapper
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
	TArray<FS_TaskRequirement> Requirements;

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
struct FS_QuestRequirement
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
struct FS_Quest
{
	GENERATED_BODY()

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(Categories="Flow.Quests"))
	FGameplayTag QuestID;

    /**Name of the quest presented to the player*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	FText QuestName;
	
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	FText QuestText;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FS_QuestTask> Tasks;

	/**Requirements to accept the quest.*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FS_QuestRequirement> Requirements;

	/**What scenarios will fail this quest?*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FS_QuestFailCondition> FailConditions;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FS_Reward> Rewards;
};

//If you add any data into this struct, remember to go into
//FL_QuestHelpers and update WrapQuest.
USTRUCT(BlueprintType)
struct FS_QuestWrapper
{
	GENERATED_BODY()

	/**The graph that made the quest
	 * Might not be valid if the graph has been destroyed.*/
	UPROPERTY(Category = "Quest", BlueprintReadOnly)
	UFlowAsset* Graph = nullptr;

	/**The node in the graph that started the quest
	 * Might not be valid if the owning graph has been destroyed.*/
	UPROPERTY(Category = "Quest", BlueprintReadOnly)
	UFlowNode* ParentNode = nullptr;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(Categories="Flow.Quests"))
	FGameplayTag QuestID;

    /**Name of the quest presented to the player*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	FText QuestName;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FS_TaskWrapper> Tasks;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FS_QuestRequirement> Requirements;

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

	bool operator==(const FS_QuestWrapper& Argument) const
	{
		if(!QuestID.IsValid() || !Argument.QuestID.IsValid())
		{
			return false;
		}

		return QuestID == Argument.QuestID;
	}

	bool operator!=(const FS_QuestWrapper& Argument) const
	{
		if(!QuestID.IsValid() || !Argument.QuestID.IsValid())
		{
			return false;
		}

		return QuestID != Argument.QuestID;
	}
};

//----------//
