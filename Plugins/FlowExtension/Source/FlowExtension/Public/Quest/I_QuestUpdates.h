// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/FE_CommonData.h"
#include "UObject/Interface.h"
#include "I_QuestUpdates.generated.h"

// This class does not need to be modified.
UINTERFACE()
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
	void QuestStateUpdated(FS_QuestWrapper Quest, EQuestState NewState);

	UFUNCTION(Category = "Quest System", BlueprintNativeEvent, BlueprintCallable)
	void QuestDropped(FS_QuestWrapper Quest);

	UFUNCTION(Category = "Quest System", BlueprintNativeEvent, BlueprintCallable)
	void QuestFailed(FS_QuestWrapper Quest);

	UFUNCTION(Category = "Quest System|Task", BlueprintNativeEvent, BlueprintCallable)
	void TaskProgressed(FS_TaskWrapper Task, float ProgressMade, UObject* Instigator);

	UFUNCTION(Category = "Quest System|Task", BlueprintNativeEvent, BlueprintCallable)
	void TaskDropped(FS_TaskWrapper Task);

	UFUNCTION(Category = "Quest System|Task", BlueprintNativeEvent, BlueprintCallable)
	void TaskFailed(FS_TaskWrapper Task);
};
