// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "DialogueManager_SubSystem.generated.h"

/**
 * 
 */
UCLASS()
class AMBIENTDIALOGUEMANAGER_API UDialogueManager_SubSystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:

	/**Ambient dialogue that has been played, but we don't want the same
	 * dialogue to repeat. If dialogue is in this list, it should not play.*/
	UPROPERTY()
	TArray<TSoftObjectPtr<USoundBase>> TrackedDialogue;
	
	void AddDialogueToTrackedList(TSoftObjectPtr<USoundBase> DialogueToTrack);

	UFUNCTION()
	void DialogueTimer(TSoftObjectPtr<USoundBase> DialogueToClear);
};
