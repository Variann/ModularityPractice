﻿// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FE_QuestData.h"
#include "I_QuestUpdates.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UI_QuestUpdates : public UInterface
{
	GENERATED_BODY()
};


/**Quests and tasks can have "listeners". When the quest or task
 * is updated in any way, the listeners should be notified via
 * this interface.
 */

class FLOWEXTENSION_API II_QuestUpdates
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(Category = "Quest System", BlueprintNativeEvent, BlueprintCallable)
	void QuestStateUpdated(FQuestWrapper Quest, EQuestState NewState);

	UFUNCTION(Category = "Quest System", BlueprintNativeEvent, BlueprintCallable)
	void QuestDropped(FQuestWrapper Quest);

	UFUNCTION(Category = "Quest System", BlueprintNativeEvent, BlueprintCallable)
	void QuestFailed(FQuestWrapper Quest);

	UFUNCTION(Category = "Quest System|Task", BlueprintNativeEvent, BlueprintCallable)
	void TaskProgressed(FTaskWrapper Task, float ProgressMade, UObject* Instigator);

	UFUNCTION(Category = "Quest System|Task", BlueprintImplementableEvent, BlueprintCallable)
	bool PreventTaskProgress(FTaskWrapper Task);

	UFUNCTION(Category = "Quest System|Task", BlueprintNativeEvent, BlueprintCallable)
	void TaskDropped(FTaskWrapper Task);

	UFUNCTION(Category = "Quest System|Task", BlueprintNativeEvent, BlueprintCallable)
	void TaskFailed(FTaskWrapper Task);

	UFUNCTION(Category = "Quest System|Task", BlueprintNativeEvent, BlueprintCallable)
	void TaskAddedToQuest(FTaskWrapper Task, FQuestWrapper Quest);

	UFUNCTION(Category = "Quest System|Task", BlueprintNativeEvent, BlueprintCallable)
	void TaskRemovedFromQuest(FTaskWrapper Task, FQuestWrapper Quest);
};
