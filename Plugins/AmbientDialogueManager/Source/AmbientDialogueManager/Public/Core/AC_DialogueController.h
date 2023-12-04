// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "O_AmbientDialogueRequirement.h"
#include "Components/ActorComponent.h"
#include "Engine/StreamableManager.h"
#include "AC_DialogueController.generated.h"

class UDA_AmbientDialogue;
struct FStreamableHandle;

UCLASS(ClassGroup=(Dialogue), DisplayName = "Dialogue Controller (ADM)", meta=(BlueprintSpawnableComponent))
class AMBIENTDIALOGUEMANAGER_API UAC_DialogueController : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAC_DialogueController();

	UPROPERTY(Category = "ADM", BlueprintReadWrite, EditAnywhere)
	TArray<UDA_AmbientDialogue*> AmbientDialogues;

	/**Find the first component on this actor that has this tag and attach
	 * the sound.*/
	UPROPERTY(Category = "ADM", BlueprintReadWrite, EditAnywhere)
	FName ComponentToAttachSoundTo;

	/**We only want to find the component once and reuse it. Though
	 * this will be reset if the component is ever destroyed and
	 * needs to be re-found.*/
	UPROPERTY()
	TObjectPtr<USceneComponent> CachedAttachToComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UAudioComponent> CurrentDialogueAudio = nullptr;

	FStreamableManager StreamableManager;
	TSharedPtr<FStreamableHandle> DialogueLoadingHandle;

	UFUNCTION(Category = "ADM", BlueprintCallable, BlueprintPure)
	TArray<UDA_AmbientDialogue*> GetAllPlayableDialogues();

	/**Get a random dialogue from the @AmbientDialogues array and play it.*/
	UFUNCTION(Category = "ADM", BlueprintCallable)
	void PlayRandomDialogue(bool AsyncLoad = true);

	UFUNCTION(Category = "ADM", BlueprintCallable)
	void PlayAmbientDialogue(UDA_AmbientDialogue* Dialogue, bool Async = true);

protected:
	
	void PlaySound_Internal(UDA_AmbientDialogue* Dialogue, USceneComponent* AttachToComponent);

	UFUNCTION()
	void DialogueFinished();
};
