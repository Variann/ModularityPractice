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

UCLASS()
class AMBIENTDIALOGUEMANAGER_API UDA_AmbientDialogue : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<USoundBase> DialogueSound = nullptr;

	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EDialoguePriority> Priority;

	/**For this dialogue to be playable, all of these requirements must be met.*/
	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere, Instanced)
	TArray<UO_AmbientDialogueRequirement*> Requirements;

	/**The minimum and maximum timer range for when this dialogue is playable again.*/
	UPROPERTY(Category = "Dialogue", BlueprintReadWrite, EditAnywhere)
	FVector2D TimerRange;

	UPROPERTY(Category = "ADM", BlueprintReadWrite, EditAnywhere)
	USoundAttenuation* SoundAttenuation = nullptr;

	UFUNCTION(Category = "ADM", BlueprintCallable, BlueprintPure)
	bool IsPlayable(AActor* Actor);

	UFUNCTION(Category = "ADM", BlueprintCallable, BlueprintPure)
	bool AreRequirementsMet(AActor* Actor);
};
