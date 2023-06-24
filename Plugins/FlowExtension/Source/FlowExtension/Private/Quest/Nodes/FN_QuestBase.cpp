// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/Nodes/FN_QuestBase.h"

UFN_QuestBase::UFN_QuestBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Quest");
#endif
}
