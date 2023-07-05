// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/FGN_Dialogue.h"
#include "Dialogue/Nodes/FN_DialogueBase.h"

#include "Slate/SFGN_Dialogue.h"

UFGN_Dialogue::UFGN_Dialogue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AssignedNodeClasses = {UFN_DialogueBase::StaticClass()};
}

TSharedPtr<SGraphNode> UFGN_Dialogue::CreateVisualWidget()
{
	return SNew(SFGN_Dialogue, this);
}
