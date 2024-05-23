// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FE_QuestData.generated.h"

class UO_TaskFailConditionBase;
class UO_TaskRequirementBase;
class UDA_Quest;

UENUM(BlueprintType)
enum EQuestState
{
	//The player has not interacted
	//with this quest or task yet.
	Inactive,
	
	InProgress,
	Completed,
	Failed
};

//----------//
//	 Task	//

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
	float ProgressRequired = 1;

	//Requirements for progressing this task.
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, Instanced)
	TArray<UO_TaskRequirementBase*> Requirements;

	/**What scenarios will fail this task?*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, Instanced)
	TArray<UO_TaskFailConditionBase*> FailConditions;

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	bool IsOptional = false;
};

//If you add any data into this struct, remember to go into
//FL_QuestHelpers and update WrapTask.
USTRUCT(BlueprintType)
struct FTaskWrapper
{
	GENERATED_BODY()

	//The quest this task belongs to.
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	UDA_Quest* RootQuest = nullptr;
	
	/**The task itself*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, meta=(Categories="Flow.Quests"))
	FGameplayTag TaskID;

    /**Name of the task presented to the player*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	FText TaskName;

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	float CurrentProgress = 0;

	/**What state is the task currently in?*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EQuestState> State = Inactive;

	/**Arbitrary data which can be added to a task, such as a timer, repeat count, etc.*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, meta=(ForceInlineRow), meta=(Categories="Flow.Quests.Metadata"))
	TMap<FGameplayTag, float> Metadata;

	/**When the task receives interface updates, these objects will receive the same
	 * interface call.
	 * This might be wiped when loading from a save.*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UObject>> Listeners;
	
	bool IsValid() const
	{
		return TaskID.IsValid();
	}
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
	TArray<TObjectPtr<UObject>> Listeners;

	bool operator==(const FQuestWrapper& Argument) const
	{
		return QuestAsset == Argument.QuestAsset;
	}

	bool operator!=(const FQuestWrapper& Argument) const
	{
		return QuestAsset != Argument.QuestAsset;
	}
};

//----------//