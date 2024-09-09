// Copyright (C) Varian Daemon 2023. All Rights Reserved.


#include "StateTree/Conditions/STC_ObjectTagQuery.h"

#include "ObjectTags_Subsystem.h"
#include "Kismet/KismetSystemLibrary.h"

bool USTC_ObjectTagQuery::TestCondition(FStateTreeExecutionContext& Context) const
{
	if(!Object || TagQuery.IsEmpty())
	{
		UKismetSystemLibrary::PrintString(this, "ObjectTagQuery condition had no object or no tag query");
		return false;
	}

	FObjectTag ObjectTag = UObjectTags_Subsystem::GetObjectTags(Object);
	return TagQuery.Matches(ObjectTag.GetTagsAsContainer());
}
