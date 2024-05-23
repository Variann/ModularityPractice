// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DA_Quest.h"
#include "Engine/DataAsset.h"
#include "DA_QuestChain.generated.h"


class UQuestChain;

USTRUCT(Blueprintable)
struct FQuestChainStage
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "Quest Chain stage", BlueprintReadWrite, EditAnywhere)
	TArray<TSoftObjectPtr<UDA_Quest>> Quests;
};

UCLASS()
class FLOWEXTENSION_API UDA_QuestChain : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "Quest Chain", BlueprintReadOnly, EditAnywhere)
	FText ChainName;
	
	UPROPERTY(Category = "Quest Chain", BlueprintReadOnly, EditAnywhere)
	TArray<FQuestChainStage> Stages;

#if WITH_EDITOR

	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif
	
};