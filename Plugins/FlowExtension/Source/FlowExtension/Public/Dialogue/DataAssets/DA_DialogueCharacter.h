// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_DialogueCharacter.generated.h"

/**
 * 
 */
UCLASS()
class FLOWEXTENSION_API UDA_DialogueCharacter : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "Visuals", BlueprintReadOnly, EditAnywhere)
	UTexture2D* CharacterPortrait = nullptr;

	UPROPERTY(Category = "Visuals", BlueprintReadOnly, EditAnywhere)
	FText Name;

	
#if WITH_EDITORONLY_DATA
	
	UPROPERTY(Category = "Developer Settings", BlueprintReadOnly, EditAnywhere, meta = (DevelopmentOnly))
	UTexture2D* FlowNodePortrait = nullptr;

	UPROPERTY(Category = "Developer Settings", BlueprintReadOnly, EditAnywhere, meta = (DevelopmentOnly))
	FLinearColor NodeColor;
	
#endif
	
};
