// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/FGN_Quest.h"
#include "Quest/Nodes/FN_QuestBase.h"

#include "Slate/SFGN_Quest.h"

UFGN_Quest::UFGN_Quest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AssignedNodeClasses = {UFN_QuestBase::StaticClass()};
}

TSharedPtr<SGraphNode> UFGN_Quest::CreateVisualWidget()
{
	return SNew(SFGN_Quest, this);
}
