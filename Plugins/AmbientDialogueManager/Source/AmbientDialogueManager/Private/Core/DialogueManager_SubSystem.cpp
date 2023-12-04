// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/DialogueManager_SubSystem.h"

#include "Components/AudioComponent.h"
#include "Core/AC_DialogueController.h"
#include "Core/DA_AmbientDialogue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UDialogueManager_SubSystem::AddDialogueToTrackedList(TSoftObjectPtr<UDA_AmbientDialogue> DialogueToTrack)
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
	float ClearTime = UKismetMathLibrary::RandomFloatInRange(DialogueToTrack.LoadSynchronous()->TimerRange.X, DialogueToTrack.LoadSynchronous()->TimerRange.Y);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, Delegate, ClearTime,false);
	TrackedDialogue.Add(DialogueToTrack);
}

void UDialogueManager_SubSystem::DialogueTimer(TSoftObjectPtr<UDA_AmbientDialogue> DialogueToClear)
{
	TrackedDialogue.RemoveSingle(DialogueToClear);
}

void UDialogueManager_SubSystem::AdjustLowerPriorityVolumes(TEnumAsByte<EDialoguePriority> Priority)
{
	for(auto& CurrentDialogue : ActiveDialogues)
	{
		if(CurrentDialogue->CurrentPlayingDialogue->Priority <= Priority)
		{
			CurrentDialogue->StopDialogue();
		}
	}
}

void UDialogueManager_SubSystem::StopAllAmbientDialogues(UObject* WorldContext)
{
	UDialogueManager_SubSystem* DialogueManager = UGameplayStatics::GetPlayerController(WorldContext, 0)->GetLocalPlayer()->GetSubsystem<UDialogueManager_SubSystem>();

	for(auto& CurrentDialogueComponent : DialogueManager->ActiveDialogues)
	{
		if(CurrentDialogueComponent)
		{
			CurrentDialogueComponent.Get()->StopDialogue();
		}
	}
}

void UDialogueManager_SubSystem::RestoreAmbientDialoguesVolume(UObject* WorldContext)
{
	UDialogueManager_SubSystem* DialogueManager = UGameplayStatics::GetPlayerController(WorldContext, 0)->GetLocalPlayer()->GetSubsystem<UDialogueManager_SubSystem>();

	for(auto& CurrentDialogueComponent : DialogueManager->ActiveDialogues)
	{
		if(CurrentDialogueComponent)
		{
			CurrentDialogueComponent.Get()->AudioComponent->AdjustVolume(0.5, 1);
		}
	}
}

TEnumAsByte<EDialoguePriority> UDialogueManager_SubSystem::GetHighestDialoguePriority(UObject* WorldContext)
{
	UDialogueManager_SubSystem* DialogueManager = UGameplayStatics::GetPlayerController(WorldContext, 0)->GetLocalPlayer()->GetSubsystem<UDialogueManager_SubSystem>();
	TEnumAsByte<EDialoguePriority> HighestPriority = Background;
	
	for(auto& CurrentDialogueComponent : DialogueManager->ActiveDialogues)
	{
		if(CurrentDialogueComponent)
		{
			if(CurrentDialogueComponent.Get()->CurrentPlayingDialogue->Priority > HighestPriority)
			{
				HighestPriority = CurrentDialogueComponent.Get()->CurrentPlayingDialogue->Priority;
			}
		}
	}

	return HighestPriority;
}
