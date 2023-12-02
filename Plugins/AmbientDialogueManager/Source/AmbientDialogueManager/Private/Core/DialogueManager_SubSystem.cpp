// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/DialogueManager_SubSystem.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UDialogueManager_SubSystem::AddDialogueToTrackedList(TSoftObjectPtr<USoundBase> DialogueToTrack)
{
	if(TrackedDialogue.Contains(DialogueToTrack))
	{
		UKismetSystemLibrary::PrintString(this, "TrackedDialogue already contained DialogueToPlay");
		return;
	}

	FTimerHandle TimerHandle;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "DialogueTimer", DialogueToTrack);

	/**A random clear time is important, because if all options are exhausted,
	 * then they might replay in the same order as they did previously as only
	 * one option will be available, forcing it to choose that option.
	 * Randomizing the clear time will help prevent the same order occuring
	 * multiple times in a row.*/
	float ClearTime = UKismetMathLibrary::RandomFloatInRange(120, 420);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, Delegate, ClearTime,false);
	TrackedDialogue.Add(DialogueToTrack);
}

void UDialogueManager_SubSystem::DialogueTimer(TSoftObjectPtr<USoundBase> DialogueToClear)
{
	TrackedDialogue.RemoveSingle(DialogueToClear);
}
