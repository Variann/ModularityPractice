// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/AC_DialogueController.h"

#include "Components/AudioComponent.h"
#include "Core/DA_AmbientDialogue.h"
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


TArray<UDA_AmbientDialogue*> UAC_DialogueController::GetAllPlayableDialogues()
{
	TArray<UDA_AmbientDialogue*> PlayableDialogues;
	if(DialogueLoadingHandle.IsValid())
	{
		UKismetSystemLibrary::PrintString(this, "Tried playing dialogue while loading another dialogue");
		return PlayableDialogues;
	}

	if(CurrentDialogueAudio)
	{
		//Busy playing dialogue
		return PlayableDialogues;
	}
	
	const UDialogueManager_SubSystem* DialogueManager = UGameplayStatics::GetPlayerController(this, 0)->GetLocalPlayer()->GetSubsystem<UDialogueManager_SubSystem>();

	for(auto& CurrentDialogue : AmbientDialogues)
	{
		if(DialogueManager->TrackedDialogue.Contains(CurrentDialogue))
		{
			//Dialogue file has been played recently, skip it.
			continue;
		}
		
		if(!CurrentDialogue->AreRequirementsMet(GetOwner()))
		{
			//Requirements aren't met, skip it
			continue;
		}
		
		PlayableDialogues.Add(CurrentDialogue);
	}
	
	return PlayableDialogues;
}

void UAC_DialogueController::PlayRandomDialogue(bool AsyncLoad)
{
	TArray<UDA_AmbientDialogue*> PlayableDialogues = GetAllPlayableDialogues();
	if(!PlayableDialogues.IsValidIndex(0))
	{
		return;
	}

	UDA_AmbientDialogue* DialogueToPlay = PlayableDialogues[UKismetMathLibrary::RandomIntegerInRange(0, PlayableDialogues.Num() - 1)];
	PlayAmbientDialogue(DialogueToPlay, AsyncLoad);
}

void UAC_DialogueController::PlayAmbientDialogue(UDA_AmbientDialogue* Dialogue, bool Async)
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
		DialogueLoadingHandle = StreamableManager.RequestAsyncLoad(Dialogue->DialogueSound.ToSoftObjectPath(), [this, Dialogue]()
		{
			PlaySound_Internal(Dialogue, CachedAttachToComponent.Get());
			DialogueLoadingHandle = nullptr;
		});
	}
	else
	{
		Dialogue->DialogueSound.LoadSynchronous();
		PlaySound_Internal(Dialogue, CachedAttachToComponent.Get());
	}
}

void UAC_DialogueController::PlaySound_Internal(UDA_AmbientDialogue* Dialogue, USceneComponent* AttachToComponent)
{
	UDialogueManager_SubSystem* DialogueManager = UGameplayStatics::GetPlayerController(this, 0)->GetLocalPlayer()->GetSubsystem<UDialogueManager_SubSystem>();

	if(!Dialogue->IsPlayable(GetOwner()))
	{
		return;
	}
	
	if(CurrentDialogueAudio.Get())
	{
		CurrentDialogueAudio->Stop();
	}
	
	CurrentDialogueAudio = UGameplayStatics::SpawnSoundAttached(Dialogue->DialogueSound.Get(), AttachToComponent);
	CurrentDialogueAudio->Stop(); //Spawn will auto play the sound and ignore our attenuation and any custom settings. Stop it and resume after applying.
	CurrentDialogueAudio->AttenuationSettings = Dialogue->SoundAttenuation;
	CurrentDialogueAudio->bOverrideAttenuation = true;
	CurrentDialogueAudio->bStopWhenOwnerDestroyed = true;
	CurrentDialogueAudio->OnAudioFinished.AddDynamic(this, &UAC_DialogueController::DialogueFinished);
	CurrentDialogueAudio->Play();
	DialogueManager->ActiveDialogues.Add(this);
	
	DialogueManager->AddDialogueToTrackedList(Dialogue);
}

void UAC_DialogueController::DialogueFinished()
{
	UDialogueManager_SubSystem* DialogueManager = UGameplayStatics::GetPlayerController(this, 0)->GetLocalPlayer()->GetSubsystem<UDialogueManager_SubSystem>();
	CurrentDialogueAudio = nullptr;
	DialogueManager->ActiveDialogues.RemoveSingle(this);
}
