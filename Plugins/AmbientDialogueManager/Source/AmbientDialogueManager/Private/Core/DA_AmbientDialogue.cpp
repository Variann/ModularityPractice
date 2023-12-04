// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "Core/DA_AmbientDialogue.h"

#include "O_AmbientDialogueRequirement.h"
#include "Core/AC_DialogueController.h"
#include "Core/DialogueManager_SubSystem.h"
#include "Kismet/GameplayStatics.h"

bool UDA_AmbientDialogue::IsPlayable(AActor* Actor)
{
	UDialogueManager_SubSystem* DialogueManager = UGameplayStatics::GetPlayerController(Actor, 0)->GetLocalPlayer()->GetSubsystem<UDialogueManager_SubSystem>();
	
	if(DialogueManager->TrackedDialogue.Contains(this))
	{
		//Check if the dialogue has been played too recently
		return false;
	}
	
	UAC_DialogueController* DialogueController = Cast<UAC_DialogueController>(Actor->GetComponentByClass(UAC_DialogueController::StaticClass()));

	if(DialogueController->AudioComponent)
	{
		//Actor is playing dialogue, can't play
		return false;
	}
	

	return true;
}

bool UDA_AmbientDialogue::AreRequirementsMet(AActor* Actor)
{
	for(auto& CurrentRequirement : Requirements)
	{
		if(!CurrentRequirement->IsConditionMet(Actor))
		{
			return false;
		}
	}

	return true;
}
