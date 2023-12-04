// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_AmbientDialogue.generated.h"

class UO_AmbientDialogueRequirement;

UENUM(BlueprintType)
enum EDialoguePriority
{
	Background,
	Low,
	Medium,
	High,
	Critical
};

UENUM()
enum EPriorityOverrideEvent
{
	LowerVolume,
	StopDialogue
};

UCLASS()
class AMBIENTDIALOGUEMANAGER_API UDA_AmbientDialogue : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<USoundBase> DialogueSound = nullptr;

	/**If a higher priority dialogue wants to activate, this dialogue
	 * will stop.*/
	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EDialoguePriority> Priority;

	/**What should happen when a higher priority dialogue wants to play?*/
	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EPriorityOverrideEvent> PriorityOverrideEvent;

	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere, meta=(EditCondition="PriorityOverrideEvent==EPriorityOverrideEvent::LowerVolume"
		, EditConditionHides, ClampMin=0, ClampMax=1, UIMin=0, UIMax=1))
	float VolumePercentageGoal = 0.1;

	/**For this dialogue to be playable, all of these requirements must be met.*/
	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere, Instanced)
	TArray<UO_AmbientDialogueRequirement*> Requirements;

	/**The minimum and maximum timer range for when this dialogue is playable again.*/
	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere)
	FVector2D TimerRange = FVector2D(120, 300);

	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere)
	USoundAttenuation* SoundAttenuation = nullptr;

	UFUNCTION(Category = "ADM", BlueprintCallable, BlueprintPure)
	bool IsPlayable(AActor* Actor);

	UFUNCTION(Category = "ADM", BlueprintCallable, BlueprintPure)
	bool AreRequirementsMet(AActor* Actor);
};
