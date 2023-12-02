// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "O_AmbientDialogueRequirement.h"
#include "Components/ActorComponent.h"
#include "Engine/StreamableManager.h"
#include "AC_DialogueController.generated.h"

struct FStreamableHandle;

UENUM(BlueprintType)
enum EDialoguePriority
{
	Background,
	Low,
	Medium,
	High,
	Critical
};

USTRUCT(BlueprintType)
struct FAmbientDialogue
{
	GENERATED_BODY()

	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<USoundBase> DialogueSound = nullptr;

	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EDialoguePriority> Priority;

	/**For this dialogue to be playable, all of these requirements must be met.*/
	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere, Instanced)
	TArray<UO_AmbientDialogueRequirement*> Requirements;
};

UCLASS(ClassGroup=(Dialogue), DisplayName = "Dialogue Controller (ADM)", meta=(BlueprintSpawnableComponent))
class AMBIENTDIALOGUEMANAGER_API UAC_DialogueController : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAC_DialogueController();

	UPROPERTY(Category = "ADM", BlueprintReadWrite, EditAnywhere)
	TArray<FAmbientDialogue> AmbientDialogues;

	/**Find the first component on this actor that has this tag and attach
	 * the sound.*/
	UPROPERTY(Category = "ADM", BlueprintReadWrite, EditAnywhere)
	FName ComponentToAttachSoundTo;

	UPROPERTY(Category = "ADM", BlueprintReadWrite, EditAnywhere)
	USoundAttenuation* SoundAttenuation = nullptr;

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
	TArray<FAmbientDialogue> GetAllPlayableDialogues();

	/**Get a random dialogue from the @AmbientDialogues array and play it.*/
	UFUNCTION(Category = "ADM", BlueprintCallable)
	void PlayRandomDialogue(bool AsyncLoad = true);

	UFUNCTION(Category = "ADM", BlueprintCallable)
	void PlayAmbientDialogue(const FAmbientDialogue& Dialogue, bool Async = true);

protected:
	
	void PlaySound_Internal(FAmbientDialogue Dialogue, USceneComponent* AttachToComponent);
};
