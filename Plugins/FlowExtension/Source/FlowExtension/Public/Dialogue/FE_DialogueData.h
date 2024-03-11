// Copyright (C) Varian Daemon. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FE_DialogueData.generated.h"

class UO_DialogueConditionBase;

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