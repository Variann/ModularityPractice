// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/AC_DialogueController.h"

#include "Components/AudioComponent.h"
#include "Core/DialogueManager_SubSystem.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UAC_DialogueController::UAC_DialogueController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


TArray<FAmbientDialogue> UAC_DialogueController::GetAllPlayableDialogues()
{
	TArray<FAmbientDialogue> PlayableDialogues;
	if(DialogueLoadingHandle.IsValid())
	{
		UKismetSystemLibrary::PrintString(this, "Tried playing dialogue while loading another dialogue");
		return PlayableDialogues;
	}
	UDialogueManager_SubSystem* DialogueManager = UGameplayStatics::GetPlayerController(this, 0)->GetLocalPlayer()->GetSubsystem<UDialogueManager_SubSystem>();

	for(auto& CurrentDialogue : AmbientDialogues)
	{
		//Dialogue file has been played recently, skip it.
		if(DialogueManager->TrackedDialogue.Contains(CurrentDialogue.DialogueSound))
		{
			continue;
		}
		
		bool RequirementFailed = false;
		for(auto& CurrentRequirement : CurrentDialogue.Requirements)
		{
			if(!CurrentRequirement->IsConditionMet(GetOwner()))
			{
				RequirementFailed = true;
				break;
			}
		}
		
		if(!RequirementFailed)
		{
			PlayableDialogues.Add(CurrentDialogue);
		}
	}
	
	return PlayableDialogues;
}

void UAC_DialogueController::PlayRandomDialogue(bool AsyncLoad)
{
	TArray<FAmbientDialogue> PlayableDialogues = GetAllPlayableDialogues();
	if(!PlayableDialogues.IsValidIndex(0))
	{
		return;
	}

	FAmbientDialogue DialogueToPlay = PlayableDialogues[UKismetMathLibrary::RandomIntegerInRange(0, PlayableDialogues.Num() - 1)];
	PlayAmbientDialogue(DialogueToPlay, AsyncLoad);
}

void UAC_DialogueController::PlayAmbientDialogue(const FAmbientDialogue& Dialogue, bool Async)
{
	if(DialogueLoadingHandle.IsValid())
	{
		UKismetSystemLibrary::PrintString(this, "Tried playing dialogue while loading another dialogue");
		return;
	}
	
	if(!CachedAttachToComponent.Get())
	{
		//Get the first found component
		TArray<UActorComponent*> FoundComponents = GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), ComponentToAttachSoundTo);
		if(FoundComponents.IsValidIndex(0))
		{
			CachedAttachToComponent = Cast<USceneComponent>(FoundComponents[0]);
		}
		else
		{
			UKismetSystemLibrary::PrintString(this, "No component with ComponentToAttachSoundTo tag was found");
			return;
		}
	}

	if(Async)
	{
		DialogueLoadingHandle = StreamableManager.RequestAsyncLoad(Dialogue.DialogueSound.ToSoftObjectPath(), [this, Dialogue]()
		{
			PlaySound_Internal(Dialogue, CachedAttachToComponent.Get());
			DialogueLoadingHandle = nullptr;
		});
	}
	else
	{
		Dialogue.DialogueSound.LoadSynchronous();
		PlaySound_Internal(Dialogue, CachedAttachToComponent.Get());
	}
}

void UAC_DialogueController::PlaySound_Internal(FAmbientDialogue Dialogue, USceneComponent* AttachToComponent)
{
	UDialogueManager_SubSystem* DialogueManager = UGameplayStatics::GetPlayerController(this, 0)->GetLocalPlayer()->GetSubsystem<UDialogueManager_SubSystem>();

	//Check if the dialogue has been played too recently
	if(DialogueManager->TrackedDialogue.Contains(Dialogue.DialogueSound.Get()))
	{
		return;
	}
	
	if(CurrentDialogueAudio.Get())
	{
		CurrentDialogueAudio->Stop();
	}
	
	CurrentDialogueAudio = UGameplayStatics::SpawnSoundAttached(Dialogue.DialogueSound.Get(), AttachToComponent);
	CurrentDialogueAudio->Stop(); //Spawn will auto play the sound and ignore our attenuation and any custom settings. Stop it and resume after applying.
	CurrentDialogueAudio->AttenuationSettings = SoundAttenuation;
	CurrentDialogueAudio->bOverrideAttenuation = true;
	CurrentDialogueAudio->bStopWhenOwnerDestroyed = true;
	CurrentDialogueAudio->Play();
	
	DialogueManager->AddDialogueToTrackedList(Dialogue.DialogueSound);
}
