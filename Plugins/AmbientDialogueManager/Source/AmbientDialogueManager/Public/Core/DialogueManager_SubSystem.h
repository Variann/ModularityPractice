// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "DialogueManager_SubSystem.generated.h"

enum EDialoguePriority : int;
class UDA_AmbientDialogue;
class UAC_DialogueController;
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
	TArray<TSoftObjectPtr<UDA_AmbientDialogue>> TrackedDialogue;

	UPROPERTY()
	TArray<TObjectPtr<UAC_DialogueController>> ActiveDialogues;
	
	void AddDialogueToTrackedList(TSoftObjectPtr<UDA_AmbientDialogue> DialogueToTrack);

	UFUNCTION()
	void DialogueTimer(TSoftObjectPtr<UDA_AmbientDialogue> DialogueToClear);

	void AdjustLowerPriorityVolumes(TEnumAsByte<EDialoguePriority> Priority);

	UFUNCTION(Category = "ADM", BlueprintCallable)
	static void StopAllAmbientDialogues(UObject* WorldContext);

	/**Restore all ambient dialogues who have had their volume reduced
	 * due to either @StopAllAmbientDialogue being called or a higher
	 * priority ambient dialogue being spoken.*/
	UFUNCTION(Category = "ADM", BlueprintCallable)
	static void RestoreAmbientDialoguesVolume(UObject* WorldContext);

	UFUNCTION(Category = "ADM", BlueprintCallable)
	static TEnumAsByte<EDialoguePriority> GetHighestDialoguePriority(UObject* WorldContext);
};
