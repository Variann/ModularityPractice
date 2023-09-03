// Copyright (C) Varian Daemon 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DA_Quest.h"
#include "Engine/DataAsset.h"
#include "DA_QuestChain.generated.h"


class UQuestChain;

UCLASS()
class FLOWEXTENSION_API UDA_QuestChain : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(Instanced, EditInstanceOnly)
	UQuestChain* QuestChain;
};

UCLASS(Blueprintable, BlueprintType, AutoExpandCategories = ("Default"), EditInlineNew)
class FLOWEXTENSION_API UQuestChain : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditInstanceOnly)
	UDA_Quest* Quest = nullptr;

	UPROPERTY(Instanced, EditInstanceOnly)
	TArray<UQuestChain*> ChainedQuests;
};