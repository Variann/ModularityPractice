// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "FE_CommonData.generated.h"

class UFE_QuestRequirement;
class UFE_Reward;
class UWidget;
class UFE_QuestTask;
class UFE_Quest;
class UO_DialogueOverrideBase;

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
	FText ButtonText;

	/**The actual dialogue that occurs when the player presses
	 * the dialogue button.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FText DialogueText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	bool Optional;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	bool Test;
};

USTRUCT(BlueprintType)
struct FS_DialogueOption
{
	GENERATED_BODY()
	
	/**The actual dialogue that occurs when the player presses
	 * the dialogue button.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FS_DialogueSettings DialogueSettings;

	/**If any of these objects override conditions return true, their dialogue
	 * settings will be used.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Dialogue Option")
	TArray<UO_DialogueOverrideBase*> OptionOverrides;
};

USTRUCT(BlueprintType)
struct FS_Script
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Option")
	FText DialogueText;

	/**If any of these objects override conditions return true, their dialogue
	 * settings will be used.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Dialogue Option")
	TArray<UO_DialogueOverrideBase*> OptionOverrides;
};


//----------//


//----------//
//	 Task	//

USTRUCT(BlueprintType)
struct FS_TaskReward
{
	GENERATED_BODY()

	// UPROPERTY(Category = "Reward", EditAnywhere, BlueprintReadOnly)
	// TSubclassOf<UFE_Reward> Task;

	UPROPERTY(Category = "Reward", EditAnywhere, BlueprintReadOnly)
	bool bIsAccepted = false;
};

USTRUCT(BlueprintType)
struct FS_TaskRequirement
{
	GENERATED_BODY()

	// UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	// TSubclassOf<UFE_> Task;

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	bool bIsCompleted = false;
};

USTRUCT(BlueprintType)
struct FS_QuestTask
{
	GENERATED_BODY()

	/**The task itself*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, meta=(Categories="Flow.Quests"))
	FGameplayTag Task;

	/**How much progress does this task require?*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, meta=(Categories="Flow.Quests.State"))
	float ProgressRequired;

	/**What scenarios will fail this task?*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(ForceInlineRow), meta=(Categories="Flow.Quests"))
	TMap<FGameplayTag, float> FailStates;
};

USTRUCT(BlueprintType)
struct FS_TaskWrapper
{
	GENERATED_BODY()
	
	/**The task itself*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, meta=(Categories="Flow.Quests"))
	FGameplayTag Task;

	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	TArray<FS_TaskRequirement> Requirements;

	/**What state is the task currently in?*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, meta=(Categories="Flow.Quests.State"))
	FGameplayTag State;

	/**Arbitrary data which can be added to a task, such as a timer, repeat count, etc.*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly, meta=(ForceInlineRow), meta=(Categories="Flow.Quests.Metadata"))
	TMap<FGameplayTag, float> Metadata;

	/**When the task receives interface updates, these widgets will receive the same
	 * interface call.*/
	UPROPERTY(Category = "Task", EditAnywhere, BlueprintReadOnly)
	TArray<UWidget*> ExternalWidgets;
};

//----------//


//----------//
//	Quest	//

USTRUCT(BlueprintType)
struct FS_QuestRequirement
{
	GENERATED_BODY()

	// UPROPERTY(Category = "Quest", Instanced, EditAnywhere, BlueprintReadOnly)
	// UFE_QuestRequirement* QuestRequirement;

	UPROPERTY(Category = "Quest", BlueprintReadOnly)
	bool bIsCompleted = false;
};

USTRUCT(BlueprintType)
struct FS_QuestWrapper
{
	GENERATED_BODY()

	// UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	// TSubclassOf<UFE_Quest> Quest;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FS_TaskWrapper> Tasks;

	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<FS_QuestRequirement> Requirements;

	/**What state is the quest currently in?*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(Categories="Flow.Quests.State"))
	FGameplayTag State;

	/**Arbitrary data which can be added to a quest, such as a timer, repeat count, etc.*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly, meta=(ForceInlineRow), meta=(Categories="Flow.Quests.Metadata"))
	TMap<FGameplayTag, float> Metadata;

	/**When the quest receives interface updates, these widgets will receive the same
	 * interface call.*/
	UPROPERTY(Category = "Quest", EditAnywhere, BlueprintReadOnly)
	TArray<UWidget*> ExternalWidgets;
};

//----------//
